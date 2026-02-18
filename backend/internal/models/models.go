package models

import "time"

// Function represents a detected libft function
type Function struct {
	Name             string `json:"name"`
	Filename         string `json:"filename"`
	IsBonus          bool   `json:"is_bonus"`
	NorminetteErrors int    `json:"norminette_errors,omitempty"`
	NorminetteOutput string `json:"norminette_output,omitempty"`
}

// NorminetteError represents a single norminette error
type NorminetteError struct {
	File    string `json:"file"`
	Line    int    `json:"line"`
	Column  int    `json:"column"`
	Message string `json:"message"`
	Rule    string `json:"rule,omitempty"`
}

// NorminetteResult represents the overall norminette check result
type NorminetteResult struct {
	HasErrors    bool                         `json:"has_errors"`
	TotalErrors  int                          `json:"total_errors"`
	Errors       []NorminetteError            `json:"errors"`
	ErrorsByFile map[string][]NorminetteError `json:"errors_by_file"`
}

// TestRequest represents a test run request
type TestRequest struct {
	SessionID       string   `json:"session_id"`
	Functions       []string `json:"functions"`
	RunMemoryChecks *bool    `json:"run_memory_checks,omitempty"`
}

// TestResult represents the result of testing a single function
type TestResult struct {
	Function        string          `json:"function"`
	Status          string          `json:"status"` // "pass", "fail", "error"
	Compilation     CompileInfo     `json:"compilation"`
	Tests           []TestCase      `json:"tests"`
	MemoryCheck     MemoryCheck     `json:"memory_check"`
	SanitizerCheck  Sanitizer       `json:"sanitizer_check"`
	Protection      Protection      `json:"protection"`
	NorminetteCheck NorminetteCheck `json:"norminette_check"`
	StageDurations  StageDurations  `json:"stage_durations,omitempty"`
	Hints           []string        `json:"hints,omitempty"`
	Duration        string          `json:"duration"`
}

// NorminetteCheck holds norminette results for a specific function
type NorminetteCheck struct {
	HasErrors bool              `json:"has_errors"`
	Errors    []NorminetteError `json:"errors"`
}

// StageDurations captures runtime spent in each phase for one function test.
type StageDurations struct {
	Compile    string `json:"compile,omitempty"`
	Tests      string `json:"tests,omitempty"`
	Valgrind   string `json:"valgrind,omitempty"`
	Sanitizer  string `json:"sanitizer,omitempty"`
	Protection string `json:"protection,omitempty"`
}

// CompileInfo holds compilation results
type CompileInfo struct {
	Success bool   `json:"success"`
	Output  string `json:"output"`
	Command string `json:"command"`
}

// TestCase represents a single test case
type TestCase struct {
	Name     string `json:"name"`
	Passed   bool   `json:"passed"`
	Expected string `json:"expected"`
	Got      string `json:"got"`
	Info     string `json:"info,omitempty"`
	Source   string `json:"source,omitempty"`
}

// MemoryCheck holds valgrind results
type MemoryCheck struct {
	HasLeaks    bool   `json:"has_leaks"`
	LeakSummary string `json:"leak_summary"`
	BytesLost   int64  `json:"bytes_lost"`
	BlocksLost  int    `json:"blocks_lost"`
	ErrorCount  int    `json:"error_count"`
	RawOutput   string `json:"raw_output"`
}

// Sanitizer holds AddressSanitizer/UBSan results
type Sanitizer struct {
	HasErrors bool     `json:"has_errors"`
	Errors    []string `json:"errors"`
	RawOutput string   `json:"raw_output"`
}

// Protection checks if function handles edge cases
type Protection struct {
	NullInput       *bool `json:"null_input,omitempty"`
	ZeroLen         *bool `json:"zero_len,omitempty"`
	IntOverflow     *bool `json:"int_overflow,omitempty"`
	EmptyString     *bool `json:"empty_string,omitempty"`
	MallocFailFirst *bool `json:"malloc_fail_first,omitempty"`
	MallocFailMid   *bool `json:"malloc_fail_mid,omitempty"`
}

// DetectResponse is returned when files are uploaded
type DetectResponse struct {
	SessionID  string            `json:"session_id"`
	Functions  []Function        `json:"functions"`
	HasBonus   bool              `json:"has_bonus"`
	Norminette *NorminetteResult `json:"norminette,omitempty"`
}

// Report is the full test report
type Report struct {
	SessionID string       `json:"session_id"`
	Timestamp time.Time    `json:"timestamp"`
	Results   []TestResult `json:"results"`
	Summary   Summary      `json:"summary"`
}

// Summary aggregates test results
type Summary struct {
	Total         int `json:"total"`
	Passed        int `json:"passed"`
	Failed        int `json:"failed"`
	Errors        int `json:"errors"`
	LeakCount     int `json:"leak_count"`
	SanitizerHits int `json:"sanitizer_hits"`
}

// SSEEvent represents a server-sent event
type SSEEvent struct {
	Type string      `json:"type"` // "progress", "result", "done", "error"
	Data interface{} `json:"data"`
}

// ProgressEvent is sent during test execution
type ProgressEvent struct {
	Function string `json:"function"`
	Step     string `json:"step"` // "compiling", "testing", "valgrind", "sanitizer", "done"
	Current  int    `json:"current"`
	Total    int    `json:"total"`
}
