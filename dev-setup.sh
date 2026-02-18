#!/bin/bash
# Quick start script for local development

set -e

echo "🚀 Libft Tester — Local Development Setup"
echo ""

# Check dependencies
check_cmd() {
    if ! command -v $1 &> /dev/null; then
        echo "❌ $1 not found. Please install it."
        return 1
    fi
}

echo "Checking dependencies..."
check_cmd go || exit 1
check_cmd node || exit 1
check_cmd gcc || echo "⚠️  gcc not found — backend tests won't run"
check_cmd valgrind || echo "⚠️  valgrind not found — memory checks won't work"

echo "✓ Dependencies OK"
echo ""

# Setup
if [ ! -d "backend/node_modules" ] && [ -f "backend/go.mod" ]; then
    echo "Setting up backend..."
    cd backend
    go mod tidy
    cd ..
fi

if [ ! -d "frontend/node_modules" ]; then
    echo "Installing frontend dependencies..."
    cd frontend
    npm install
    cd ..
fi

echo ""
echo "✓ Setup complete!"
echo ""
echo "To start development:"
echo ""
echo "  Terminal 1 (Backend):"
echo "    cd backend"
echo "    go run ./cmd/server"
echo ""
echo "  Terminal 2 (Frontend):"
echo "    cd frontend"
echo "    npm run dev"
echo ""
echo "Then open: http://localhost:5173"
echo ""
echo "To test with a sample libft:"
echo "    ./create-sample.sh"
echo ""
