# Libft Tester

Web-based tester for the 42 Libft project. Drag & drop your code, get instant reports with compilation errors, memory leaks, and undefined behavior detection.

## Quick Start (Docker)

```bash
# Pull and run (single command)
docker run -p 5173:5173 ghcr.io/42staverni/libft-ghost:latest

# Open http://localhost:5173 in your browser
```

## Quick Start (Development)

```bash
# Terminal 1: Backend
cd backend && go run ./cmd/server

# Terminal 2: Frontend
cd frontend && npm run dev

# Open http://localhost:5173
```

## How It Works

1. **Upload** — Drag & drop your libft folder or zip
2. **Select** — Pick which functions to test
3. **Test** — Real-time progress via SSE streaming
4. **Report** — See compilation, tests, memory leaks, buffer overflows, edge cases

## Features

- ✅ **Folder drop** — No zipping needed
- ✅ **Real-time** — Live progress updates
- ✅ **Comprehensive** — Compilation, tests, Valgrind, ASan/UBSan
- ✅ **Fast mode** — Skip memory checks for 50-100ms per test
- ✅ **Norminette** — Style checking built-in
- ✅ **Glassmorphism UI** — Modern, minimalist design
- ✅ **Single container** — One Docker image, no dependencies

## Project Structure

```
backend/          # Go HTTP API
├── cmd/server/   # Entry point
├── internal/
│   ├── handler/  # Upload + test endpoints
│   ├── runner/   # Test execution pipeline
│   └── testcases/# C test harnesses
└── Dockerfile

frontend/         # SvelteKit SPA
├── src/lib/components/  # UI components
└── src/routes/          # Main app
```

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

## Distribution (GHCR)

Students just run the published image. No installation needed.

### For Students

```bash
# Run the tester (any platform with Docker)
docker run -p 5173:5173 ghcr.io/42staverni/libft-ghost:latest

# Then open http://localhost:5173
```

### For Developers (Publishing to GHCR)

1. **Build the image:**
   ```bash
   docker build -t ghcr.io/42staverni/libft-ghost:latest .
   ```

2. **Authenticate with GitHub Container Registry:**
   ```bash
   # Using personal access token (classic) with 'write:packages' scope
   echo $GITHUB_TOKEN | docker login ghcr.io -u YOUR_USERNAME --password-stdin
   ```

3. **Push the image:**
   ```bash
   docker push ghcr.io/42staverni/libft-ghost:latest
   ```

4. **Make the package public** (if needed):
   - Go to GitHub → Your profile → Packages
   - Click on `libft-ghost`
   - Package settings → Change visibility → Public

### Alternative: Multi-arch Build

For cross-platform support (Intel + Apple Silicon):

```bash
# Create and use buildx builder
docker buildx create --use

# Build for multiple platforms
docker buildx build \
  --platform linux/amd64,linux/arm64 \
  -t ghcr.io/42staverni/libft-ghost:latest \
  --push .
```

## Requirements

**For development only:**
- Go 1.25+, Node 22+
- gcc, make, valgrind, norminette
- For Valgrind: `sudo apt-get install libc6-dbg`

**For end users:**
- Docker only!

## API

- `POST /api/upload` — Upload zip/folder
- `POST /api/test` — Run tests (SSE streaming)
- `GET /api/health` — Health check

## Architecture

```
┌──────────────────────────────────────────┐
│          Docker Container                │
│  ┌─────────────┐     ┌─────────────┐    │
│  │   Svelte    │────▶│     Go      │    │
│  │  (static)   │     │  (server)   │    │
│  │             │◄────│             │    │
│  └─────────────┘     └──────┬──────┘    │
│                    gcc/valgrind/sanitizer│
└──────────────────────────────────────────┘
```

## Troubleshooting

**Backend won't start:** `cd backend && go mod tidy && go build ./...`

**Frontend crashes:** `cd frontend && npm install && npm run dev`

**No functions detected:** Ensure `ft_*.c` files are at zip root

**Valgrind errors:** Install `sudo apt-get install libc6-dbg`

**CORS errors:** Add your frontend URL to `backend/cmd/server/main.go` allowedOrigins

**Docker port already in use:**
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
    // Output: PASS|FAIL test_name expected got
    printf("PASS basic_test 5 %d\n", ft_strlen("hello"));
    return 0;
}
```

## License

MIT
