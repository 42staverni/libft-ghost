package handler

import (
	"context"
	"encoding/json"
	"fmt"
	"io"
	"io/fs"
	"net/http"
	"os"
	"path/filepath"
	"runtime"
	"strings"
	"sync"
	"time"

	"github.com/libftui/backend/internal/models"
	"github.com/libftui/backend/internal/runner"
)

const maxUploadSize = 10 * 1024 * 1024 // 10MB

// Handler holds dependencies for HTTP handlers
type Handler struct {
	TestCasesDir string
	Workers      int
	RunnerLimits runner.Limits
}

// NewHandler creates a new handler
func NewHandler(testCasesDir string, workers int, limits runner.Limits) *Handler {
	if workers <= 0 {
		workers = maxWorkersDefault()
	}
	if limits.CompileWorkers <= 0 {
		limits.CompileWorkers = workers
	}
	if limits.ProtectionWorkers <= 0 {
		limits.ProtectionWorkers = workers
	}
	if limits.ValgrindWorkers <= 0 {
		limits.ValgrindWorkers = workers
	}
	if limits.SanitizerWorkers <= 0 {
		limits.SanitizerWorkers = workers
	}
	return &Handler{TestCasesDir: testCasesDir, Workers: workers, RunnerLimits: limits}
}

// Upload handles file upload, extraction, and function detection
func (h *Handler) Upload(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
		return
	}

	r.Body = http.MaxBytesReader(w, r.Body, maxUploadSize)
	if err := r.ParseMultipartForm(maxUploadSize); err != nil {
		http.Error(w, "file too large (max 10MB)", http.StatusBadRequest)
		return
	}

	file, _, err := r.FormFile("project")
	if err != nil {
		http.Error(w, "missing 'project' file field", http.StatusBadRequest)
		return
	}
	defer file.Close()

	// Create a temp directory for this session
	sessionDir, err := os.MkdirTemp("", "libft-*")
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}

	// Save uploaded zip
	zipPath := filepath.Join(sessionDir, "upload.zip")
	out, err := os.Create(zipPath)
	if err != nil {
		http.Error(w, "internal error", http.StatusInternalServerError)
		return
	}
	if _, err := io.Copy(out, file); err != nil {
		out.Close()
		http.Error(w, "failed to save upload", http.StatusInternalServerError)
		return
	}
	out.Close()

	// Extract
	extractDir := filepath.Join(sessionDir, "src")
	rootDir, err := runner.ExtractZip(zipPath, extractDir)
	if err != nil {
		http.Error(w, "failed to extract zip: "+err.Error(), http.StatusBadRequest)
		return
	}

	// Detect functions
	functions, err := runner.DetectFunctions(rootDir)
	if err != nil {
		http.Error(w, "failed to detect functions: "+err.Error(), http.StatusInternalServerError)
		return
	}

	if len(functions) == 0 {
		// Fall back to recursive search for nested layouts (e.g. project/ft_functions)
		if subFuncs, subDir := detectFunctionsInTree(rootDir); len(subFuncs) > 0 {
			rootDir = subDir
			functions = subFuncs
		}
	}

	hasBonus := false
	for _, f := range functions {
		if f.IsBonus {
			hasBonus = true
			break
		}
	}

	// Run norminette check
	normResult := runner.CheckNorminette(r.Context(), rootDir)

	// Save norminette results to session directory for later use
	if normResult != nil {
		normFile := filepath.Join(sessionDir, "norminette.json")
		if data, err := json.Marshal(normResult); err == nil {
			os.WriteFile(normFile, data, 0644)
		}
	}

	// Map norminette errors to functions
	if normResult != nil && len(normResult.Errors) > 0 {
		for i := range functions {
			funcName := functions[i].Name
			filename := functions[i].Filename
			normCheck := runner.GetNorminetteErrorsForFunction(normResult, funcName, filename)
			functions[i].NorminetteErrors = len(normCheck.Errors)
		}
	}

	sessionID := filepath.Base(sessionDir)
	resp := models.DetectResponse{
		SessionID:  sessionID,
		Functions:  functions,
		HasBonus:   hasBonus,
		Norminette: normResult,
	}

	// Save function list to session for later use
	funcFile := filepath.Join(sessionDir, "functions.json")
	if data, err := json.Marshal(functions); err == nil {
		os.WriteFile(funcFile, data, 0644)
	}

	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(resp)
}

// RunTests handles test execution with SSE streaming
func (h *Handler) RunTests(w http.ResponseWriter, r *http.Request) {
	if r.Method != http.MethodPost {
		http.Error(w, "method not allowed", http.StatusMethodNotAllowed)
		return
	}

	var req models.TestRequest
	if err := json.NewDecoder(r.Body).Decode(&req); err != nil {
		http.Error(w, "invalid request body", http.StatusBadRequest)
		return
	}

	sessionDir := filepath.Join(os.TempDir(), req.SessionID)
	if _, err := os.Stat(sessionDir); os.IsNotExist(err) {
		http.Error(w, "session not found — upload first", http.StatusNotFound)
		return
	}

	// Load norminette results from session
	var normResult *models.NorminetteResult
	normFile := filepath.Join(sessionDir, "norminette.json")
	if data, err := os.ReadFile(normFile); err == nil {
		json.Unmarshal(data, &normResult)
	}

	// Find source directory in the extracted tree, then resolve build directory
	srcDir := filepath.Join(sessionDir, "src")
	workDir := srcDir
	_, sourceDir := detectFunctionsInTree(srcDir)
	if sourceDir != "" {
		workDir = resolveBuildDir(sourceDir, srcDir)
	}

	// Get function to filename mapping from cached file
	funcToFile := make(map[string]string)
	funcFile := filepath.Join(sessionDir, "functions.json")
	if data, err := os.ReadFile(funcFile); err == nil {
		var functions []models.Function
		if json.Unmarshal(data, &functions) == nil {
			for _, f := range functions {
				funcToFile[f.Name] = f.Filename
			}
		}
	}

	// Set up SSE
	w.Header().Set("Content-Type", "text/event-stream")
	w.Header().Set("Cache-Control", "no-cache")
	w.Header().Set("Connection", "keep-alive")
	w.Header().Set("Access-Control-Allow-Origin", "*")
	flusher, ok := w.(http.Flusher)
	if !ok {
		http.Error(w, "streaming not supported", http.StatusInternalServerError)
		return
	}

	r2 := runner.NewRunner(workDir, h.TestCasesDir, h.RunnerLimits)
	total := len(req.Functions)
	runMemoryChecks := true
	if req.RunMemoryChecks != nil {
		runMemoryChecks = *req.RunMemoryChecks
	}

	// Check if any bonus functions are requested
	bonusSet := make(map[string]bool)
	for _, f := range runner.BonusFunctions {
		bonusSet[f] = true
	}
	needsBonus := false
	for _, f := range req.Functions {
		if bonusSet[f] {
			needsBonus = true
			break
		}
	}

	// Step 1: Run make (and make bonus if needed) to build libft.a
	sendSSE(w, flusher, "progress", models.ProgressEvent{
		Function: "libft",
		Step:     "make",
		Current:  0,
		Total:    total,
	})
	makeResult := r2.Make(r.Context(), needsBonus)
	sendSSE(w, flusher, "make", makeResult)

	if !makeResult.Success {
		// make failed — report error for all functions
		results := make([]models.TestResult, 0, total)
		for _, funcName := range req.Functions {
			result := models.TestResult{
				Function:    funcName,
				Status:      "error",
				Compilation: makeResult,
				Hints:       []string{"make failed — fix compilation errors in your Makefile first."},
			}
			// Add norminette check to result
			if normResult != nil {
				filename := funcToFile[funcName]
				result.NorminetteCheck = runner.GetNorminetteErrorsForFunction(normResult, funcName, filename)
			}
			results = append(results, result)
			sendSSE(w, flusher, "result", result)
		}
		summary := buildSummary(results)
		report := models.Report{
			SessionID: req.SessionID,
			Timestamp: time.Now(),
			Results:   results,
			Summary:   summary,
		}
		sendSSE(w, flusher, "done", report)
		return
	}

	workers := h.Workers
	if workers > total {
		workers = total
	}
	if workers < 1 {
		workers = 1
	}

	type job struct {
		index int
		name  string
	}
	type out struct {
		index  int
		result models.TestResult
	}

	jobs := make(chan job)
	started := make(chan job, total)
	outputs := make(chan out, total)
	var wg sync.WaitGroup

	// Set per-function timeout based on mode
	// Quick mode: ~500ms per test + margin = 10s
	// Full mode: includes valgrind/sanitizer = 60s
	funcTimeout := 10 * time.Second
	if runMemoryChecks {
		funcTimeout = 60 * time.Second
	}

	for i := 0; i < workers; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			defer func() {
				if rec := recover(); rec != nil {
					fmt.Printf("worker panic recovered: %v\n", rec)
				}
			}()
			for j := range jobs {
				started <- j
				ctx, cancel := context.WithTimeout(r.Context(), funcTimeout)
				res := r2.TestFunction(ctx, j.name, runMemoryChecks)
				cancel()
				outputs <- out{index: j.index, result: res}
			}
		}()
	}

	go func() {
		for i, funcName := range req.Functions {
			jobs <- job{index: i, name: funcName}
		}
		close(jobs)
		wg.Wait()
		close(started)
		close(outputs)
	}()

	// Use ordered slice directly - avoid double storage
	ordered := make([]models.TestResult, total)
	startedCount := 0
	completed := 0
	outputsClosed := false
	for completed < total {
		select {
		case s, ok := <-started:
			if !ok {
				started = nil
				continue
			}
			startedCount++
			sendSSE(w, flusher, "progress", models.ProgressEvent{
				Function: s.name,
				Step:     "testing",
				Current:  startedCount,
				Total:    total,
			})
		case output, ok := <-outputs:
			if !ok {
				outputs = nil
				outputsClosed = true
				break
			}
			// Add norminette check to result
			if normResult != nil {
				filename := funcToFile[output.result.Function]
				output.result.NorminetteCheck = runner.GetNorminetteErrorsForFunction(normResult, output.result.Function, filename)
			}
			ordered[output.index] = output.result
			sendSSE(w, flusher, "result", output.result)
			completed++
		}

		if outputsClosed {
			break
		}
	}

	// Handle incomplete results directly in ordered slice
	if completed < total {
		for i, funcName := range req.Functions {
			if ordered[i].Function != "" {
				continue
			}
			ordered[i] = models.TestResult{
				Function: funcName,
				Status:   "error",
				Hints:    []string{"Test execution ended unexpectedly before this function finished."},
			}
			sendSSE(w, flusher, "result", ordered[i])
		}
	}

	// Filter non-empty results directly without allocation
	finalResults := ordered[:0] // Reuse underlying array
	for _, result := range ordered {
		if result.Function != "" {
			finalResults = append(finalResults, result)
		}
	}

	// Send summary
	summary := buildSummary(finalResults)
	report := models.Report{
		SessionID: req.SessionID,
		Timestamp: time.Now(),
		Results:   finalResults,
		Summary:   summary,
	}
	sendSSE(w, flusher, "done", report)

	// Cleanup
	go func() {
		time.Sleep(5 * time.Minute)
		os.RemoveAll(sessionDir)
	}()
}

func sendSSE(w http.ResponseWriter, flusher http.Flusher, eventType string, data interface{}) {
	jsonData, _ := json.Marshal(data)
	fmt.Fprintf(w, "event: %s\ndata: %s\n\n", eventType, jsonData)
	flusher.Flush()
}

func buildSummary(results []models.TestResult) models.Summary {
	s := models.Summary{Total: len(results)}
	for _, r := range results {
		switch r.Status {
		case "pass":
			s.Passed++
		case "fail":
			s.Failed++
		case "error":
			s.Errors++
		}
		if r.MemoryCheck.HasLeaks {
			s.LeakCount++
		}
		if r.SanitizerCheck.HasErrors {
			s.SanitizerHits++
		}
	}
	return s
}

// Health returns a simple health check
func (h *Handler) Health(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "application/json")
	json.NewEncoder(w).Encode(map[string]string{"status": "ok"})
}

func detectFunctionsInTree(root string) ([]models.Function, string) {
	bestFuncs := []models.Function{}
	bestDir := ""

	_ = filepath.WalkDir(root, func(path string, d fs.DirEntry, err error) error {
		if err != nil || !d.IsDir() {
			return nil
		}

		funcs, detectErr := runner.DetectFunctions(path)
		if detectErr != nil {
			return nil
		}

		if len(funcs) > len(bestFuncs) {
			bestFuncs = funcs
			bestDir = path
		}

		return nil
	})

	return bestFuncs, bestDir
}

func resolveBuildDir(sourceDir, srcRoot string) string {
	dir := sourceDir
	for {
		if _, err := os.Stat(filepath.Join(dir, "Makefile")); err == nil {
			return dir
		}

		if dir == srcRoot {
			break
		}

		parent := filepath.Dir(dir)
		if parent == dir {
			break
		}

		rel, err := filepath.Rel(srcRoot, parent)
		if err != nil || strings.HasPrefix(rel, "..") {
			break
		}

		dir = parent
	}

	return sourceDir
}

func maxWorkersDefault() int {
	cpu := runtime.NumCPU()
	if cpu < 2 {
		return 1
	}
	workers := cpu / 2
	if workers < 2 {
		workers = 2
	}
	if workers > 6 {
		workers = 6
	}
	return workers
}
