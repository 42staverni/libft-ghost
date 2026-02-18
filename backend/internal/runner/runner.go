package runner

import (
	"bytes"
	"context"
	"fmt"
	"io/fs"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"

	"github.com/libftui/backend/internal/models"
)

const (
	makeTimeout = 30 * time.Second

	// Timeouts for "quick mode" (no memory checks) - much faster
	quickCompileTimeout    = 2 * time.Second
	quickTestTimeout       = 500 * time.Millisecond
	quickProtectionTimeout = 1 * time.Second

	// Timeouts for "full mode" (with memory checks)
	fullCompileTimeout    = 5 * time.Second
	fullTestTimeout       = 5 * time.Second
	fullValgrindTimeout   = 15 * time.Second
	fullSanitizerTimeout  = 10 * time.Second
	fullProtectionTimeout = 5 * time.Second
)

// Pre-compiled regex patterns for valgrind parsing
var (
	valgrindLeakRegex  = regexp.MustCompile(`definitely lost: ([\d,]+) bytes in (\d+) blocks`)
	valgrindErrorRegex = regexp.MustCompile(`ERROR SUMMARY: (\d+) errors`)
)

// Runner executes tests against a libft submission
type Runner struct {
	WorkDir      string // Student's extracted code
	TestCasesDir string // Pre-written test harnesses
	MadeOnce     bool   // Whether `make` has been run
	MakeResult   models.CompileInfo
	Limits       Limits

	cacheMu          sync.RWMutex
	headerDirCache   string
	headerDirReady   bool
	sourceFilesCache []string
	sourceFilesReady bool

	compileSem    chan struct{}
	valgrindSem   chan struct{}
	sanitizerSem  chan struct{}
	protectionSem chan struct{}
}

// Limits controls concurrency of heavy runner stages.
type Limits struct {
	CompileWorkers    int
	ValgrindWorkers   int
	SanitizerWorkers  int
	ProtectionWorkers int
}

// NewRunner creates a new test runner

func NewRunner(workDir, testCasesDir string, limits Limits) *Runner {
	normalized := normalizeLimits(limits)
	return &Runner{
		WorkDir:       workDir,
		TestCasesDir:  testCasesDir,
		Limits:        normalized,
		compileSem:    makeLimiter(normalized.CompileWorkers),
		valgrindSem:   makeLimiter(normalized.ValgrindWorkers),
		sanitizerSem:  makeLimiter(normalized.SanitizerWorkers),
		protectionSem: makeLimiter(normalized.ProtectionWorkers),
	}
}

// Make runs `make` (and optionally `make bonus`) in the student's directory.
func (r *Runner) Make(ctx context.Context, needsBonus bool) models.CompileInfo {
	if r.MadeOnce {
		return r.MakeResult
	}
	r.MadeOnce = true

	ctx, cancel := context.WithTimeout(ctx, makeTimeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, "make", "-C", r.WorkDir)
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &out

	err := cmd.Run()
	r.MakeResult = models.CompileInfo{
		Success: err == nil,
		Output:  out.String(),
		Command: fmt.Sprintf("make -C %s", r.WorkDir),
	}

	// Verify libft.a was actually produced
	if r.MakeResult.Success {
		if _, statErr := os.Stat(filepath.Join(r.WorkDir, "libft.a")); os.IsNotExist(statErr) {
			r.MakeResult.Success = false
			r.MakeResult.Output += "\n\nError: make succeeded but libft.a was not created."
		}
	}

	// Run make bonus if bonus functions are requested
	if r.MakeResult.Success && needsBonus {
		bonusCmd := exec.CommandContext(ctx, "make", "bonus", "-C", r.WorkDir)
		var out2 bytes.Buffer
		bonusCmd.Stdout = &out2
		bonusCmd.Stderr = &out2
		if err2 := bonusCmd.Run(); err2 != nil {
			r.MakeResult.Output += "\n\n--- make bonus ---\n" + out2.String()
			// Don't fail entirely — mandatory functions still work
		} else {
			r.MakeResult.Output += "\n\n--- make bonus ---\n" + out2.String()
			r.MakeResult.Command += " && " + fmt.Sprintf("make bonus -C %s", r.WorkDir)
		}
	}

	return r.MakeResult
}

// TestFunction runs all checks for a single function
func (r *Runner) TestFunction(ctx context.Context, funcName string, runMemoryChecks bool) models.TestResult {
	start := time.Now()
	result := models.TestResult{
		Function:       funcName,
		Status:         "pass",
		StageDurations: models.StageDurations{},
	}

	// Step 1: Verify libft.a exists (make should have been called already)
	libftA := filepath.Join(r.WorkDir, "libft.a")
	if _, err := os.Stat(libftA); os.IsNotExist(err) {
		result.Status = "error"
		result.Compilation = models.CompileInfo{
			Success: false,
			Output:  "libft.a not found — make failed or was not run.",
		}
		result.Hints = append(result.Hints, "Your Makefile must produce libft.a in the project root.")
		result.Duration = time.Since(start).String()
		return result
	}

	// Select timeouts based on mode
	compileTimeout := quickCompileTimeout
	testTimeout := quickTestTimeout
	protectionTimeout := quickProtectionTimeout
	if runMemoryChecks {
		compileTimeout = fullCompileTimeout
		testTimeout = fullTestTimeout
		protectionTimeout = fullProtectionTimeout
	}

	// Step 2: Compile and run test harness against libft.a
	if err := acquireStage(ctx, r.compileSem); err != nil {
		result.Status = "error"
		result.Compilation = models.CompileInfo{Success: false, Output: "timed out waiting for compile slot"}
		result.Hints = append(result.Hints, "Server overloaded during compile stage; retry or reduce TEST_WORKERS.")
		result.Duration = time.Since(start).String()
		return result
	}
	compileStart := time.Now()
	testBinary, compileResult := r.compileTestHarness(ctx, funcName, compileTimeout)
	releaseStage(r.compileSem)
	result.StageDurations.Compile = time.Since(compileStart).String()
	result.Compilation = compileResult
	if !compileResult.Success {
		result.Status = "error"
		result.Hints = append(result.Hints, "Test harness failed to link — check your function prototype matches the subject.")
		result.Duration = time.Since(start).String()
		return result
	}

	// Step 3: Run tests
	testStart := time.Now()
	result.Tests = r.runTests(ctx, testBinary, funcName, testTimeout)
	result.StageDurations.Tests = time.Since(testStart).String()
	for _, t := range result.Tests {
		if !t.Passed {
			result.Status = "fail"
		}
	}

	if runMemoryChecks {
		// Step 4: Valgrind memory check
		if err := acquireStage(ctx, r.valgrindSem); err != nil {
			result.Status = "error"
			result.Hints = append(result.Hints, "Timed out waiting for valgrind slot.")
			result.Duration = time.Since(start).String()
			return result
		}
		valgrindStart := time.Now()
		result.MemoryCheck = r.runValgrind(ctx, funcName, fullValgrindTimeout, fullCompileTimeout)
		releaseStage(r.valgrindSem)
		result.StageDurations.Valgrind = time.Since(valgrindStart).String()
		if strings.Contains(result.MemoryCheck.RawOutput, "Fatal error at startup") ||
			strings.Contains(result.MemoryCheck.RawOutput, "Cannot continue -- exiting now") {
			if result.Status == "pass" {
				result.Status = "error"
			}
			result.Hints = append(result.Hints, "Valgrind could not run in this environment (install libc debug symbols, e.g. libc6-dbg).")
		}
		if result.MemoryCheck.HasLeaks {
			result.Hints = append(result.Hints, "Memory leak detected. Ensure every malloc() has a corresponding free().")
		}

		// Step 5: AddressSanitizer check (recompile from source with sanitizers)
		if err := acquireStage(ctx, r.sanitizerSem); err != nil {
			result.Status = "error"
			result.Hints = append(result.Hints, "Timed out waiting for sanitizer slot.")
			result.Duration = time.Since(start).String()
			return result
		}
		sanitizerStart := time.Now()
		result.SanitizerCheck = r.runSanitizer(ctx, funcName, fullSanitizerTimeout, fullCompileTimeout)
		releaseStage(r.sanitizerSem)
		result.StageDurations.Sanitizer = time.Since(sanitizerStart).String()
		if result.SanitizerCheck.HasErrors {
			result.Hints = append(result.Hints, "Address/UB sanitizer found issues. Check for buffer overflows and undefined behavior.")
		}
	} else {
		result.MemoryCheck = models.MemoryCheck{LeakSummary: "Skipped by user option"}
		result.SanitizerCheck = models.Sanitizer{RawOutput: "Skipped by user option"}
	}

	// Step 6: Protection checks
	if err := acquireStage(ctx, r.protectionSem); err != nil {
		result.Status = "error"
		result.Hints = append(result.Hints, "Timed out waiting for protection-check slot.")
		result.Duration = time.Since(start).String()
		return result
	}
	protectionStart := time.Now()
	result.Protection = r.checkProtection(ctx, funcName, compileTimeout, protectionTimeout)
	releaseStage(r.protectionSem)
	result.StageDurations.Protection = time.Since(protectionStart).String()
	if result.Protection.NullInput != nil && !*result.Protection.NullInput {
		result.Hints = append(result.Hints, "NULL input not handled — your function should check for NULL parameters.")
	}
	if result.Protection.MallocFailFirst != nil && !*result.Protection.MallocFailFirst {
		result.Hints = append(result.Hints, "malloc failure not handled — return NULL when allocation fails.")
	}
	if result.Protection.MallocFailMid != nil && !*result.Protection.MallocFailMid {
		result.Hints = append(result.Hints, "Partial malloc failure leaks memory — free all previous allocations before returning NULL.")
	}

	result.Duration = time.Since(start).String()
	return result
}

// compileTestHarness compiles the test harness and links it against libft.a
func (r *Runner) compileTestHarness(ctx context.Context, funcName string, timeout time.Duration) (string, models.CompileInfo) {
	ctx, cancel := context.WithTimeout(ctx, timeout)
	defer cancel()

	testSrc := filepath.Join(r.TestCasesDir, "test_"+funcName+".c")
	if _, err := os.Stat(testSrc); os.IsNotExist(err) {
		return "", models.CompileInfo{
			Success: false,
			Output:  fmt.Sprintf("No test harness found for %s", funcName),
		}
	}

	libftA := filepath.Join(r.WorkDir, "libft.a")
	binFile := filepath.Join(r.WorkDir, "test_"+funcName)
	includeDir := r.findHeaderDir()

	// Functions that compare against BSD libc equivalents
	bsdFuncs := map[string]bool{
		"ft_strlcpy": true, "ft_strlcat": true, "ft_strnstr": true,
	}
	extraFlags := ""
	if bsdFuncs[funcName] {
		extraFlags = "-lbsd"
	}

	args := []string{"-Wall", "-Wextra", "-Werror", testSrc, libftA, "-o", binFile, "-I", includeDir}
	if extraFlags != "" {
		args = append(args, extraFlags)
	}
	cmd := exec.CommandContext(ctx, "cc", args...)
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &out

	err := cmd.Run()
	return binFile, models.CompileInfo{
		Success: err == nil,
		Output:  out.String(),
		Command: "cc " + strings.Join(args, " "),
	}
}

// runTests executes the test binary and parses results
func (r *Runner) runTests(ctx context.Context, binaryPath, funcName string, timeout time.Duration) []models.TestCase {
	ctx, cancel := context.WithTimeout(ctx, timeout)
	defer cancel()

	cmd := exec.CommandContext(ctx, binaryPath)
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &out
	cmd.Run()

	tests := parseTestOutput(out.String())
	sourceByName := r.loadTestCaseSourceMap(funcName)
	for i := range tests {
		if src, ok := sourceByName[tests[i].Name]; ok {
			tests[i].Source = src
		}
	}

	return tests
}

func (r *Runner) loadTestCaseSourceMap(funcName string) map[string]string {
	result := make(map[string]string)

	testSrc := filepath.Join(r.TestCasesDir, "test_"+funcName+".c")
	content, err := os.ReadFile(testSrc)
	if err != nil {
		return result
	}

	callPattern := regexp.MustCompile(`^\s*([A-Za-z_][A-Za-z0-9_]*)\("([^"]+)"(.*)\)\s*;\s*$`)
	for _, line := range strings.Split(string(content), "\n") {
		matches := callPattern.FindStringSubmatch(line)
		if len(matches) != 4 {
			continue
		}
		testName := matches[2]
		if _, exists := result[testName]; exists {
			continue
		}
		result[testName] = strings.TrimSpace(line)
	}

	return result
}

// parseTestOutput parses structured test output (one line per test)
// Format: "PASS|FAIL test_name [expected] [got] [info]"
func parseTestOutput(output string) []models.TestCase {
	var tests []models.TestCase
	for _, line := range strings.Split(output, "\n") {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		parts := strings.SplitN(line, " ", 5)
		if len(parts) < 2 {
			continue
		}
		tc := models.TestCase{
			Name:   parts[1],
			Passed: parts[0] == "PASS",
		}
		if len(parts) >= 3 {
			tc.Expected = parts[2]
		}
		if len(parts) >= 4 {
			tc.Got = parts[3]
		}
		if len(parts) >= 5 {
			tc.Info = parts[4]
		}
		tests = append(tests, tc)
	}
	return tests
}

// runValgrind runs valgrind on the test binary
func (r *Runner) runValgrind(ctx context.Context, funcName string, valgrindTimeout, compileTimeout time.Duration) models.MemoryCheck {
	ctx, cancel := context.WithTimeout(ctx, valgrindTimeout+compileTimeout)
	defer cancel()

	binaryPath, compileOut, compileErr := r.compileHarnessFromSources(ctx, funcName, "_vg", "")
	if compileErr != nil {
		return models.MemoryCheck{
			ErrorCount:  1,
			LeakSummary: "Valgrind compilation failed",
			RawOutput:   compileOut,
		}
	}

	cmd := exec.CommandContext(ctx, "valgrind",
		"--leak-check=full",
		"--show-leak-kinds=all",
		"--track-origins=yes",
		"--error-exitcode=42",
		binaryPath)
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &out
	err := cmd.Run()

	output := out.String()
	check := models.MemoryCheck{
		RawOutput: output,
	}

	if err != nil && (strings.Contains(output, "Fatal error at startup") || strings.Contains(output, "Cannot continue -- exiting now")) {
		check.ErrorCount = 1
		check.LeakSummary = "Valgrind could not run in this environment"
		return check
	}

	// Parse valgrind output using pre-compiled regexes
	if strings.Contains(output, "definitely lost:") {
		if matches := valgrindLeakRegex.FindStringSubmatch(output); len(matches) == 3 {
			bytesStr := strings.ReplaceAll(matches[1], ",", "")
			check.BytesLost, _ = strconv.ParseInt(bytesStr, 10, 64)
			check.BlocksLost, _ = strconv.Atoi(matches[2])
			check.HasLeaks = check.BytesLost > 0
		}
	}

	if matches := valgrindErrorRegex.FindStringSubmatch(output); len(matches) == 2 {
		check.ErrorCount, _ = strconv.Atoi(matches[1])
	}

	if strings.Contains(output, "All heap blocks were freed") {
		check.LeakSummary = "No leaks detected"
	} else if check.HasLeaks {
		check.LeakSummary = fmt.Sprintf("%d bytes lost in %d blocks", check.BytesLost, check.BlocksLost)
	} else if err != nil && check.ErrorCount == 0 {
		check.ErrorCount = 1
		check.LeakSummary = "Valgrind execution failed"
	}

	return check
}

// runSanitizer recompiles from source with fsanitize and runs
func (r *Runner) runSanitizer(ctx context.Context, funcName string, testTimeout, compileTimeout time.Duration) models.Sanitizer {
	ctx, cancel := context.WithTimeout(ctx, testTimeout+compileTimeout)
	defer cancel()

	// Functions where signed integer overflow is expected (matches libc behavior)
	ubExclude := map[string]bool{"ft_atoi": true}

	sanitizeFlags := "-fsanitize=address,undefined"
	if ubExclude[funcName] {
		sanitizeFlags = "-fsanitize=address"
	}

	binFile, compileOut, err := r.compileHarnessFromSources(ctx, funcName, "_san", sanitizeFlags)
	if err != nil {
		return models.Sanitizer{
			HasErrors: false,
			RawOutput: "Sanitizer compilation skipped: " + compileOut,
		}
	}

	// Run sanitized binary
	runCmd := exec.CommandContext(ctx, binFile)
	var out bytes.Buffer
	runCmd.Stdout = &out
	runCmd.Stderr = &out
	runCmd.Env = append(os.Environ(), "ASAN_OPTIONS=detect_leaks=1")
	runCmd.Run()

	output := out.String()
	san := models.Sanitizer{
		RawOutput: output,
		HasErrors: strings.Contains(output, "ERROR:") ||
			strings.Contains(output, "runtime error:") ||
			strings.Contains(output, "heap-buffer-overflow") ||
			strings.Contains(output, "stack-buffer-overflow"),
	}

	// Extract individual errors
	for _, line := range strings.Split(output, "\n") {
		if strings.Contains(line, "ERROR:") || strings.Contains(line, "runtime error:") {
			san.Errors = append(san.Errors, strings.TrimSpace(line))
		}
	}

	return san
}

func (r *Runner) compileHarnessFromSources(ctx context.Context, funcName, suffix, extraFlag string) (string, string, error) {
	testSrc := filepath.Join(r.TestCasesDir, "test_"+funcName+".c")
	if _, err := os.Stat(testSrc); os.IsNotExist(err) {
		return "", "No test harness found for " + funcName, err
	}

	srcFiles := r.collectSourceFiles()
	if len(srcFiles) == 0 {
		return "", "No source files found for recompilation", fmt.Errorf("no source files")
	}

	includeDir := r.findHeaderDir()
	binFile := filepath.Join(r.WorkDir, "test_"+funcName+suffix)

	args := []string{"-g", "-O0"}
	if extraFlag != "" {
		args = append(args, extraFlag)
	}
	args = append(args, testSrc)
	args = append(args, srcFiles...)
	args = append(args, "-o", binFile, "-I", includeDir)

	bsdFuncs := map[string]bool{
		"ft_strlcpy": true, "ft_strlcat": true, "ft_strnstr": true,
	}
	if bsdFuncs[funcName] {
		args = append(args, "-lbsd")
	}

	cmd := exec.CommandContext(ctx, "cc", args...)
	var out bytes.Buffer
	cmd.Stdout = &out
	cmd.Stderr = &out
	err := cmd.Run()

	return binFile, out.String(), err
}

// checkProtection tests edge cases like NULL input
func (r *Runner) checkProtection(ctx context.Context, funcName string, compileTimeout, testTimeout time.Duration) models.Protection {
	prot := models.Protection{}

	testSrc := filepath.Join(r.TestCasesDir, "test_"+funcName+"_protection.c")
	if _, err := os.Stat(testSrc); os.IsNotExist(err) {
		return prot
	}

	libftA := filepath.Join(r.WorkDir, "libft.a")
	binFile := filepath.Join(r.WorkDir, "test_"+funcName+"_prot")
	includeDir := r.findHeaderDir()

	ctx, cancel := context.WithTimeout(ctx, compileTimeout+testTimeout)
	defer cancel()

	args := []string{"-Wall", "-Wextra", "-Werror", testSrc, libftA, "-o", binFile, "-I", includeDir, "-ldl"}
	cmd := exec.CommandContext(ctx, "cc", args...)
	if err := cmd.Run(); err != nil {
		return prot
	}

	runCmd := exec.CommandContext(ctx, binFile)
	var out bytes.Buffer
	runCmd.Stdout = &out
	runCmd.Stderr = &out
	runCmd.Run()

	output := out.String()
	for _, line := range strings.Split(output, "\n") {
		parts := strings.SplitN(strings.TrimSpace(line), " ", 2)
		if len(parts) != 2 {
			continue
		}
		val := parts[0] == "PASS"
		switch parts[1] {
		case "null_input":
			prot.NullInput = &val
		case "zero_len":
			prot.ZeroLen = &val
		case "int_overflow":
			prot.IntOverflow = &val
		case "empty_string":
			prot.EmptyString = &val
		case "malloc_fail_first":
			prot.MallocFailFirst = &val
		case "malloc_fail_second", "malloc_fail_third":
			prot.MallocFailMid = &val
		}
	}

	return prot
}

func (r *Runner) findHeaderDir() string {
	r.cacheMu.RLock()
	if r.headerDirReady {
		cached := r.headerDirCache
		r.cacheMu.RUnlock()
		return cached
	}
	r.cacheMu.RUnlock()

	rootHeader := filepath.Join(r.WorkDir, "libft.h")
	if _, err := os.Stat(rootHeader); err == nil {
		r.cacheMu.Lock()
		r.headerDirCache = r.WorkDir
		r.headerDirReady = true
		r.cacheMu.Unlock()
		return r.WorkDir
	}

	headerDir := ""
	_ = filepath.WalkDir(r.WorkDir, func(path string, d fs.DirEntry, err error) error {
		if err != nil || d.IsDir() {
			return nil
		}
		if d.Name() == "libft.h" {
			headerDir = filepath.Dir(path)
		}
		return nil
	})

	if headerDir != "" {
		r.cacheMu.Lock()
		r.headerDirCache = headerDir
		r.headerDirReady = true
		r.cacheMu.Unlock()
		return headerDir
	}

	r.cacheMu.Lock()
	r.headerDirCache = r.WorkDir
	r.headerDirReady = true
	r.cacheMu.Unlock()
	return r.WorkDir
}

func (r *Runner) collectSourceFiles() []string {
	r.cacheMu.RLock()
	if r.sourceFilesReady {
		cached := make([]string, len(r.sourceFilesCache))
		copy(cached, r.sourceFilesCache)
		r.cacheMu.RUnlock()
		return cached
	}
	r.cacheMu.RUnlock()

	var srcFiles []string
	_ = filepath.WalkDir(r.WorkDir, func(path string, d fs.DirEntry, err error) error {
		if err != nil || d.IsDir() {
			return nil
		}
		name := d.Name()
		if strings.HasPrefix(name, "ft_") && strings.HasSuffix(name, ".c") {
			srcFiles = append(srcFiles, path)
		}
		return nil
	})
	sort.Strings(srcFiles)

	r.cacheMu.Lock()
	r.sourceFilesCache = append([]string(nil), srcFiles...)
	r.sourceFilesReady = true
	r.cacheMu.Unlock()

	return srcFiles
}

func normalizeLimits(l Limits) Limits {
	if l.CompileWorkers < 1 {
		l.CompileWorkers = 1
	}
	if l.ValgrindWorkers < 1 {
		l.ValgrindWorkers = 1
	}
	if l.SanitizerWorkers < 1 {
		l.SanitizerWorkers = 1
	}
	if l.ProtectionWorkers < 1 {
		l.ProtectionWorkers = 1
	}
	return l
}

func makeLimiter(size int) chan struct{} {
	if size <= 0 {
		return nil
	}
	sem := make(chan struct{}, size)
	for i := 0; i < size; i++ {
		sem <- struct{}{}
	}
	return sem
}

func acquireStage(ctx context.Context, sem chan struct{}) error {
	if sem == nil {
		return nil
	}
	select {
	case <-ctx.Done():
		return ctx.Err()
	case <-sem:
		return nil
	}
}

func releaseStage(sem chan struct{}) {
	if sem == nil {
		return
	}
	sem <- struct{}{}
}
