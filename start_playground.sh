#!/usr/bin/env bash

# ==============================================================================
# 🚀 C2Go Playground: All-in-One Homelab Service Manager
# Usage:
#   ./start_playground.sh          -> Start both Backend & Frontend
#   ./start_playground.sh stop     -> Stop both services
#   ./start_playground.sh status   -> Check if services are running
# ==============================================================================

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$PROJECT_DIR/web-server"
FRONTEND_DIR="$PROJECT_DIR/web"
LOG_DIR="$PROJECT_DIR/logs"

mkdir -p "$LOG_DIR"

stop_services() {
    echo "🛑 Stopping C2Go Playground services..."
    pkill -f "go run server.go" 2>/dev/null
    pkill -f "c2go-server" 2>/dev/null
    pkill -f "vite" 2>/dev/null
    sleep 1
    echo "✅ All services stopped."
}

check_status() {
    echo "📊 Service Status:"
    if pgrep -f "server.go" > /dev/null || pgrep -f "c2go-server" > /dev/null; then
        echo "  🟢 Backend API: Running on port 8080"
    else
        echo "  🔴 Backend API: Stopped"
    fi

    if pgrep -f "vite" > /dev/null; then
        echo "  🟢 Frontend Web: Running on port 5173"
    else
        echo "  🔴 Frontend Web: Stopped"
    fi
}

case "$1" in
    stop)
        stop_services
        exit 0
        ;;
    status)
        check_status
        exit 0
        ;;
    restart)
        stop_services
        sleep 1
        ;;
esac

echo "=================================================="
echo "⚡ Starting C2Go Homelab Playground"
echo "=================================================="

# 1. Stop any stale instances
stop_services > /dev/null 2>&1

# 2. Check / Build c2go compiler binary if missing
if [ ! -f "$PROJECT_DIR/c2go" ]; then
    echo "🔨 Building c2go compiler..."
    gcc -Wall -Wextra -std=c11 -I"$PROJECT_DIR/include" \
        "$PROJECT_DIR/compiler/main.c" \
        "$PROJECT_DIR/compiler/arena.c" \
        "$PROJECT_DIR/compiler/lexer.c" \
        "$PROJECT_DIR/compiler/ast.c" \
        "$PROJECT_DIR/compiler/parser.c" \
        "$PROJECT_DIR/compiler/codegen.c" \
        -o "$PROJECT_DIR/c2go"
fi

# 3. Ensure frontend dependencies exist
if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
    echo "📦 node_modules not found. Running npm install..."
    (cd "$FRONTEND_DIR" && npm install)
fi

# 4. Start Go Backend API
echo "🚀 Starting Go API backend on :8080..."
cd "$BACKEND_DIR"
nohup go run server.go > "$LOG_DIR/backend.log" 2>&1 &
BACKEND_PID=$!

sleep 1

# 5. Determine IP addresses for easy mobile access
TAILSCALE_IP=$(tailscale ip -4 2>/dev/null || echo "127.0.0.1")

echo ""
echo "=================================================="
echo "🎉 Backend running! (PID: $BACKEND_PID)"
echo "📱 Tailscale URL for your phone: http://$TAILSCALE_IP:5173"
echo "💻 Local URL:                    http://localhost:5173"
echo "📝 Backend logs:                 tail -f logs/backend.log"
echo "=================================================="
echo "Starting Vite Frontend (Press Ctrl+C anytime to stop)..."
echo ""

cd "$FRONTEND_DIR"
exec npm run dev -- --host 0.0.0.0

