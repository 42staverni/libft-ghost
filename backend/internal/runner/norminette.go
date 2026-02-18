package runner

import (
	"context"
	"encoding/json"
	"fmt"
	"os/exec"
	"path/filepath"
	"strings"
	"time"

	"github.com/libftui/backend/internal/models"
)

const norminetteTimeout = 30 * time.Second

// norminetteOutput represents the JSON structure from norminette
type norminetteOutput struct {
	Files []norminetteFile `json:"files"`
}

type norminetteFile struct {
	Path   string            `json:"path"`
	Status string            `json:"status"`
	Errors []norminetteError `json:"errors"`
}

type norminetteError struct {
	Name       string                `json:"name"`
	Text       string                `json:"text"`
	Level      string                `json:"level"`
	Highlights []norminetteHighlight `json:"highlights"`
}

type norminetteHighlight struct {
	Lineno int     `json:"lineno"`
	Column int     `json:"column"`
	Length *int    `json:"length"`
	Hint   *string `json:"hint"`
}

// CheckNorminette runs norminette on the project directory
func CheckNorminette(ctx context.Context, dir string) *models.NorminetteResult {
	// Check if norminette is available
	if !isNorminetteAvailable() {
		return nil
	}

	ctx, cancel := context.WithTimeout(ctx, norminetteTimeout)
	defer cancel()

	// Run norminette with JSON output
	cmd := exec.CommandContext(ctx, "norminette", "-f", "json", dir)
	output, _ := cmd.CombinedOutput() // Ignore error since norminette exits with 1 when errors found

	// Filter out non-JSON lines (like "Setting locale to en_US")
	lines := strings.Split(string(output), "\n")
	var jsonLines []string
	for _, line := range lines {
		line = strings.TrimSpace(line)
		if strings.HasPrefix(line, "{") {
			jsonLines = append(jsonLines, line)
		}
	}
	jsonOutput := strings.Join(jsonLines, "")

	result := &models.NorminetteResult{
		Errors:       []models.NorminetteError{},
		ErrorsByFile: make(map[string][]models.NorminetteError),
	}

	// Parse JSON output
	var normOut norminetteOutput
	if err := json.Unmarshal([]byte(jsonOutput), &normOut); err != nil {
		// Fallback: if JSON parsing fails, try plain text format
		return parseNorminetteText(string(output))
	}

	// Process each file
	for _, file := range normOut.Files {
		if file.Status == "Error" || file.Status == "Warning" {
			// Extract base filename from absolute path
			baseFile := filepath.Base(file.Path)

			for _, err := range file.Errors {
				// Each error can have multiple highlights
				if len(err.Highlights) > 0 {
					for _, hl := range err.Highlights {
						normErr := models.NorminetteError{
							File:    baseFile,
							Line:    hl.Lineno,
							Column:  hl.Column,
							Message: err.Text,
							Rule:    err.Name,
						}
						result.Errors = append(result.Errors, normErr)
						result.ErrorsByFile[baseFile] = append(result.ErrorsByFile[baseFile], normErr)
					}
				} else {
					// Error without line/column info
					normErr := models.NorminetteError{
						File:    baseFile,
						Message: err.Text,
						Rule:    err.Name,
					}
					result.Errors = append(result.Errors, normErr)
					result.ErrorsByFile[baseFile] = append(result.ErrorsByFile[baseFile], normErr)
				}
			}
		}
	}

	result.TotalErrors = len(result.Errors)
	result.HasErrors = result.TotalErrors > 0

	return result
}

// isNorminetteAvailable checks if norminette is installed
func isNorminetteAvailable() bool {
	cmd := exec.Command("which", "norminette")
	return cmd.Run() == nil
}

// parseNorminetteText parses plain text norminette output (fallback)
func parseNorminetteText(output string) *models.NorminetteResult {
	result := &models.NorminetteResult{
		Errors:       []models.NorminetteError{},
		ErrorsByFile: make(map[string][]models.NorminetteError),
	}

	lines := strings.Split(output, "\n")
	var currentFile string

	for _, line := range lines {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}

		// Check for file status line: "filename.c: Error!" or "filename.c: OK!"
		if strings.HasSuffix(line, ": Error!") || strings.HasSuffix(line, ": OK!") {
			parts := strings.Split(line, ": ")
			if len(parts) >= 2 {
				currentFile = parts[0]
			}
			continue
		}

		// Check for error details: "Error: RULENAME (line: 23, col: 5): message"
		if strings.HasPrefix(line, "Error:") && currentFile != "" {
			// Parse error line
			// Format: "Error: SPACE_AFTER_KW (line: 23, col: 5): Missing space after keyword"
			parts := strings.SplitN(line, ":", 3)
			if len(parts) >= 3 {
				ruleName := strings.TrimSpace(parts[1])
				// Remove line/col info from rule name
				if idx := strings.Index(ruleName, "("); idx > 0 {
					ruleName = strings.TrimSpace(ruleName[:idx])
				}

				message := strings.TrimSpace(parts[2])
				// Extract line and column from parentheses if present
				var lineNum, colNum int
				if startIdx := strings.Index(parts[1], "("); startIdx >= 0 {
					if endIdx := strings.Index(parts[1], ")"); endIdx > startIdx {
						info := parts[1][startIdx+1 : endIdx]
						// Parse "line: 23, col: 5"
						if _, err := fmt.Sscanf(info, "line: %d, col: %d", &lineNum, &colNum); err != nil {
							lineNum = 0
							colNum = 0
						}
					}
				}

				normErr := models.NorminetteError{
					File:    filepath.Base(currentFile),
					Line:    lineNum,
					Column:  colNum,
					Message: message,
					Rule:    ruleName,
				}
				result.Errors = append(result.Errors, normErr)
				result.ErrorsByFile[normErr.File] = append(result.ErrorsByFile[normErr.File], normErr)
			}
		}
	}

	result.TotalErrors = len(result.Errors)
	result.HasErrors = result.TotalErrors > 0

	return result
}

// GetNorminetteErrorsForFunction returns norminette errors for a specific function based on its filename
func GetNorminetteErrorsForFunction(normResult *models.NorminetteResult, funcName, filename string) models.NorminetteCheck {
	if normResult == nil || filename == "" {
		return models.NorminetteCheck{HasErrors: false, Errors: []models.NorminetteError{}}
	}

	// Get base filename (without path)
	baseFilename := filepath.Base(filename)

	// Get errors for this file
	errors := normResult.ErrorsByFile[baseFilename]

	return models.NorminetteCheck{
		HasErrors: len(errors) > 0,
		Errors:    errors,
	}
}
