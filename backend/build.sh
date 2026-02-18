#!/bin/sh
# Build the Go binary and Docker image
set -e

echo "Building Go binary..."
CGO_ENABLED=0 GOOS=linux GOARCH=amd64 go build -o server ./cmd/server

echo "Building Docker image..."
docker build -t libftui-backend .

echo "Done! Run with: docker run -p 8080:8080 libftui-backend"
