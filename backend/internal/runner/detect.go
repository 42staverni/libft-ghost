package runner

import (
	"archive/zip"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"

	"github.com/libftui/backend/internal/models"
)

// AllFunctions is the complete list of libft mandatory functions
var AllFunctions = []string{
	"ft_isalpha", "ft_isdigit", "ft_isalnum", "ft_isascii", "ft_isprint",
	"ft_strlen", "ft_memset", "ft_bzero", "ft_memcpy", "ft_memmove",
	"ft_strlcpy", "ft_strlcat", "ft_toupper", "ft_tolower",
	"ft_strchr", "ft_strrchr", "ft_strncmp", "ft_memchr", "ft_memcmp",
	"ft_strnstr", "ft_atoi", "ft_calloc", "ft_strdup",
	"ft_substr", "ft_strjoin", "ft_strtrim", "ft_split", "ft_itoa",
	"ft_strmapi", "ft_striteri",
	"ft_putchar_fd", "ft_putstr_fd", "ft_putendl_fd", "ft_putnbr_fd",
}

// BonusFunctions is the list of bonus linked-list functions
var BonusFunctions = []string{
	"ft_lstnew", "ft_lstadd_front", "ft_lstsize", "ft_lstlast",
	"ft_lstadd_back", "ft_lstdelone", "ft_lstclear",
	"ft_lstiter", "ft_lstmap",
}

// ExtractZip extracts a zip file to the given directory and returns the root
func ExtractZip(zipPath, destDir string) (string, error) {
	r, err := zip.OpenReader(zipPath)
	if err != nil {
		return "", fmt.Errorf("failed to open zip: %w", err)
	}
	defer r.Close()

	var rootDir string
	for _, f := range r.File {
		// Sanitize path to prevent zip slip
		name := filepath.Clean(f.Name)
		if strings.Contains(name, "..") {
			continue
		}
		target := filepath.Join(destDir, name)

		if f.FileInfo().IsDir() {
			if rootDir == "" {
				rootDir = target
			}
			os.MkdirAll(target, 0755)
			continue
		}

		os.MkdirAll(filepath.Dir(target), 0755)

		outFile, err := os.OpenFile(target, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
		if err != nil {
			return "", fmt.Errorf("failed to create file %s: %w", target, err)
		}

		rc, err := f.Open()
		if err != nil {
			outFile.Close()
			return "", fmt.Errorf("failed to open file in zip: %w", err)
		}

		// Limit extraction to 10MB per file
		_, err = io.Copy(outFile, io.LimitReader(rc, 10*1024*1024))
		rc.Close()
		outFile.Close()
		if err != nil {
			return "", fmt.Errorf("failed to extract file: %w", err)
		}
	}

	if rootDir == "" {
		rootDir = destDir
	}
	return rootDir, nil
}

// DetectFunctions scans a directory for ft_*.c files and returns detected functions
func DetectFunctions(dir string) ([]models.Function, error) {
	var functions []models.Function
	knownMandatory := make(map[string]bool)
	knownBonus := make(map[string]bool)

	for _, f := range AllFunctions {
		knownMandatory[f] = true
	}
	for _, f := range BonusFunctions {
		knownBonus[f] = true
	}

	entries, err := os.ReadDir(dir)
	if err != nil {
		return nil, fmt.Errorf("failed to read directory: %w", err)
	}

	for _, entry := range entries {
		if entry.IsDir() {
			continue
		}
		name := entry.Name()
		if !strings.HasPrefix(name, "ft_") || !strings.HasSuffix(name, ".c") {
			continue
		}

		funcName := strings.TrimSuffix(name, ".c")
		isBonus := false

		// Check for bonus files
		if strings.HasSuffix(funcName, "_bonus") {
			funcName = strings.TrimSuffix(funcName, "_bonus")
			isBonus = true
		}

		if knownMandatory[funcName] || knownBonus[funcName] {
			functions = append(functions, models.Function{
				Name:     funcName,
				Filename: name,
				IsBonus:  isBonus || knownBonus[funcName],
			})
		}
	}

	return functions, nil
}
