# Stage 1: Build frontend
FROM node:22-alpine AS frontend-builder
WORKDIR /app
COPY frontend/package*.json .
RUN npm ci
COPY frontend/ .
RUN npm run build

# Stage 2: Build Go backend
FROM golang:1.25-alpine AS backend-builder
WORKDIR /app
COPY backend/go.mod backend/go.sum ./
RUN go mod download
COPY backend/ .
RUN CGO_ENABLED=1 go build -o server ./cmd/server

# Final stage
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    gcc \
    libc6-dev \
    libbsd-dev \
    valgrind \
    make \
    ca-certificates \
    curl \
    python3 \
    python3-pip \
    python3-setuptools \
    && rm -rf /var/lib/apt/lists/*

# Install norminette
RUN python3 -m pip install --break-system-packages -U norminette

WORKDIR /app
COPY --from=backend-builder /app/server .
COPY --from=frontend-builder /app/build ./static
COPY backend/testcases/ ./testcases/

ENV PORT=5173
ENV TEST_CASES_DIR=/app/testcases
ENV FRONTEND_ORIGIN=http://localhost:5173
ENV STATIC_DIR=/app/static

EXPOSE 5173

CMD ["/app/server"]
