# libft-ghost 👻

A web-based tester for the 42 Libft project. Drag & drop your code, get instant reports with compilation errors, memory leaks, and undefined behavior detection.

Built with a modern glassmorphism UI and distributed as a single Docker container.

## Quick Start

```bash
# Pull and run (single command)
docker run -p 5173:5173 ghcr.io/42staverni/libft-ghost:latest

# Open http://localhost:5173 in your browser
```

## Features

- ✅ **Single container** — Everything in one Docker image, no dependencies
- ✅ **Folder drop** — No zipping needed (drag & drop directly)
- ✅ **Real-time** — Live progress updates via Server-Sent Events
- ✅ **Comprehensive** — Compilation, tests, Valgrind, ASan/UBSan
- ✅ **Fast mode** — Skip memory checks for 50-100ms per test
- ✅ **Norminette** — 42 School style checking built-in
- ✅ **Multi-arch** — Works on Intel/AMD and Apple Silicon (ARM64)
- ✅ **Glassmorphism UI** — Modern, minimalist design with light/dark mode

## How It Works

1. **Upload** — Drag & drop your libft folder or zip file
2. **Select** — Pick which functions to test (with norminette warnings)
3. **Test** — Watch real-time progress as tests run
4. **Report** — See compilation status, test results, memory leaks, buffer overflows, and edge cases

## Testing Pipeline

For each function:
1. Compile with `-Wall -Wextra -Werror`
2. Run test harness
3. Valgrind (memory leaks) — optional
4. AddressSanitizer (buffer overflows) — optional
5. Edge case protection tests

## Timeouts

| Mode | Per-Function | Compile | Test |
|------|-------------|---------|------|
| Quick (no memory) | 10s | 2s | 500ms |
| Full (with memory) | 60s | 5s | 5s |

## Project Structure

```
.
├── Dockerfile              # Multi-stage build (frontend + backend)
├── .github/workflows/
│   └── docker.yml          # GitHub Actions: parallel multi-arch builds
├── backend/                # Go HTTP API
│   ├── cmd/server/         # Entry point
│   ├── internal/
│   │   ├── handler/        # Upload + test endpoints
│   │   ├── runner/         # Test execution pipeline
│   │   │   ├── runner.go
│   │   │   ├── norminette.go
│   │   │   └── detect.go
│   │   ├── middleware/     # CORS, rate limiting
│   │   └── testcases/      # C test harnesses (48 test files)
│   └── Dockerfile
└── frontend/               # SvelteKit SPA
    ├── src/
    │   ├── lib/components/ # DropZone, FunctionPicker, TestReport
    │   └── routes/         # Main app (+page.svelte)
    ├── Dockerfile.dev
    └── package.json
```

## Development

### Quick Start (Local)

```bash
# Terminal 1: Backend
cd backend && go run ./cmd/server

# Terminal 2: Frontend
cd frontend && npm run dev

# Open http://localhost:5173
```

### Requirements

- Go 1.25+, Node 22+
- gcc, make, valgrind
- norminette (for style checking)
- For Valgrind: `sudo apt-get install libc6-dbg`

### API Endpoints

- `GET /` — Static frontend (SPA)
- `POST /api/upload` — Upload zip/folder
- `POST /api/test` — Run tests (SSE streaming)
- `GET /api/health` — Health check

## Distribution (GHCR)

### For Students (End Users)

```bash
# Pull and run (any platform with Docker)
docker run -p 5173:5173 ghcr.io/42staverni/libft-ghost:latest

# Or use a specific version
docker run -p 5173:5173 ghcr.io/42staverni/libft-ghost:v1.0.0
```

Then open `http://localhost:5173` in your browser.

### For Maintainers (Publishing)

Images are automatically built and published to GitHub Container Registry (GHCR) via GitHub Actions:

- **Push to `main` branch** → Builds and pushes `latest` tag
- **Create a release** (e.g., `v1.0.0`) → Builds and pushes versioned tags

The workflow uses **parallel matrix builds** for faster multi-architecture support (AMD64 + ARM64).

#### Manual Build (if needed)

```bash
# Build locally
docker build -t ghcr.io/42staverni/libft-ghost:latest .

# Multi-arch build
docker buildx create --use
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  -t ghcr.io/42staverni/libft-ghost:latest \
  --push .

# Login and push
echo $GITHUB_TOKEN | docker login ghcr.io -u USERNAME --password-stdin
docker push ghcr.io/42staverni/libft-ghost:latest
```

#### Make Package Public

1. Go to GitHub → Your profile → Packages
2. Click on `libft-ghost`
3. Package settings → Change visibility → **Public**

## Troubleshooting

### Network Error When Uploading
- Make sure you're accessing `http://localhost:5173` (not `https`)
- Check Docker logs: `docker logs <container-id>`

### Backend Won't Start
```bash
cd backend && go mod tidy && go build ./...
```

### Frontend Crashes
```bash
cd frontend && npm install && npm run dev
```

### No Functions Detected
- Ensure `ft_*.c` files are at the zip/folder root
- Check that your Makefile is present

### Valgrind Errors
```bash
sudo apt-get install libc6-dbg
```

### Docker Port Already in Use
```bash
# Use a different port
docker run -p 5174:5173 ghcr.io/42staverni/libft-ghost:latest
# Then visit http://localhost:5174
```

## Adding Tests

Create `backend/testcases/test_ft_<function>.c`:

```c
#include <stdio.h>

int main(void)
{
    // Output format: PASS|FAIL test_name expected got
    printf("PASS basic_test 5 %d\n", ft_strlen("hello"));
    return 0;
}
```

## Architecture

```
┌──────────────────────────────────────────────┐
│          Docker Container                    │
│  ┌─────────────────┐  ┌─────────────────┐   │
│  │   SvelteKit     │  │   Go Server     │   │
│  │   (Frontend)    │──│   (Backend)     │   │
│  │                 │  │                 │   │
│  └─────────────────┘  └────────┬────────┘   │
│                                 │            │
│              gcc/valgrind/norminette         │
└──────────────────────────────────────────────┘
```

## License

MIT

---

**Built for 42 students by 42 students** 👻
