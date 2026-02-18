package main

import (
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strconv"
	"strings"
	"time"

	"github.com/libftui/backend/internal/handler"
	"github.com/libftui/backend/internal/middleware"
	"github.com/libftui/backend/internal/runner"
)

func main() {
	port := os.Getenv("PORT")
	if port == "" {
		port = "5173"
	}

	staticDir := os.Getenv("STATIC_DIR")
	if staticDir == "" {
		// Check if we're in development mode (frontend running separately)
		if _, err := os.Stat("frontend/build"); err == nil {
			staticDir = "frontend/build"
		} else if _, err := os.Stat("build"); err == nil {
			staticDir = "build"
		}
	}

	testCasesDir := os.Getenv("TEST_CASES_DIR")
	if testCasesDir == "" {
		if _, err := os.Stat("testcases"); err == nil {
			testCasesDir = "testcases"
		} else {
			testCasesDir = "/app/testcases"
		}
	}

	allowedOrigins := []string{
		"http://localhost:5173",
		"http://localhost:4173",
		os.Getenv("FRONTEND_ORIGIN"),
	}
	if allowedOrigins[2] == "" {
		allowedOrigins[2] = "*"
	}

	workers := 0
	if rawWorkers := os.Getenv("TEST_WORKERS"); rawWorkers != "" {
		parsedWorkers, err := strconv.Atoi(rawWorkers)
		if err != nil || parsedWorkers < 1 {
			log.Printf("Invalid TEST_WORKERS=%q, falling back to default", rawWorkers)
		} else {
			workers = parsedWorkers
		}
	}

	limits := runner.Limits{
		CompileWorkers:    readPositiveEnvInt("COMPILE_WORKERS"),
		ValgrindWorkers:   readPositiveEnvInt("VALGRIND_WORKERS"),
		SanitizerWorkers:  readPositiveEnvInt("SANITIZER_WORKERS"),
		ProtectionWorkers: readPositiveEnvInt("PROTECTION_WORKERS"),
	}

	h := handler.NewHandler(testCasesDir, workers, limits)

	mux := http.NewServeMux()

	// API routes
	mux.HandleFunc("/api/health", h.Health)
	mux.HandleFunc("/api/upload", h.Upload)
	mux.HandleFunc("/api/test", h.RunTests)

	// Static files (if built)
	if staticDir != "" {
		fs := http.FileServer(http.Dir(staticDir))
		mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
			// API routes are already handled above
			if strings.HasPrefix(r.URL.Path, "/api/") {
				return
			}

			// Try to serve the file
			path := filepath.Join(staticDir, r.URL.Path)
			if _, err := os.Stat(path); err == nil {
				fs.ServeHTTP(w, r)
				return
			}

			// SPA fallback: serve index.html for all other routes
			http.ServeFile(w, r, filepath.Join(staticDir, "index.html"))
		})
	}

	// Apply middleware
	rl := middleware.NewRateLimiter(10, 1*time.Minute)
	corsMiddleware := middleware.CORS(allowedOrigins)
	chain := corsMiddleware(rl.Middleware(mux))

	// Print startup banner
	printBanner(port, staticDir != "")

	if err := http.ListenAndServe(":"+port, chain); err != nil {
		log.Fatal(err)
	}
}

func printBanner(port string, hasStatic bool) {
	// ghost ASCII art
	banner := `
    .-.
   (o o)
   |O|
  /   \
 (  |  )
  || ||
 _|| ||_
/__/ \__\
`
	log.Printf("%s", banner)
	log.Printf("👻  libft-ghost watching over your code")
	log.Printf("🌐  Open http://localhost:%s in your browser", port)
	log.Printf("📁  Test cases: %s", os.Getenv("TEST_CASES_DIR"))
	if hasStatic {
		log.Printf("🗂️   Serving static frontend files")
	} else {
		log.Printf("🔧  API mode - connect frontend separately")
	}
}

func readPositiveEnvInt(name string) int {
	raw := os.Getenv(name)
	if raw == "" {
		return 0
	}
	value, err := strconv.Atoi(raw)
	if err != nil || value < 1 {
		log.Printf("Invalid %s=%q, ignoring", name, raw)
		return 0
	}
	return value
}
