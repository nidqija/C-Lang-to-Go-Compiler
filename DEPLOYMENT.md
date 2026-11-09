# 🛠️ C2Go: Debian / Ubuntu Homelab & Deployment Operations Guide

This runbook contains all essential commands to monitor, track, manage, and troubleshoot your self-hosted **C2Go Continuous Deployment (CD)** pipeline and remote mobile infrastructure.

---

## 📑 Table of Contents
- [1. Real-Time Deployment Tracking](#1-real-time-deployment-tracking)
- [2. Managing the Auto-Deploy Worker](#2-managing-the-auto-deploy-worker)
- [3. Git & Build Status Commands](#3-git--build-status-commands)
- [4. Remote Access & Network Management](#4-remote-access--network-management)
- [5. Testing & Verification](#5-testing--verification)
- [6. Upgrading to a Systemd Service (Auto-Start on Boot)](#6-upgrading-to-a-systemd-service-auto-start-on-boot)
- [7. Emergency & Troubleshooting Cheat Sheet](#7-emergency--troubleshooting-cheat-sheet)

---

## 1. Real-Time Deployment Tracking

### Stream Live Deployment Logs
Watch your deployment worker detect commits, pull code, and rebuild in real-time:
```bash
tail -f ~/c2go/auto_deploy.log
```
> *(Press `Ctrl + C` anytime to exit log streaming).*

### View the Last 50 Deployment Log Lines
```bash
tail -n 50 ~/c2go/auto_deploy.log
```

### Search Logs for Successful Builds or Errors
```bash
grep -E "(Successfully|Error|Pulling)" ~/c2go/auto_deploy.log
```

---

## 2. Managing the Auto-Deploy Worker

### Check if the Background Worker is Running
```bash
ps aux | grep auto_deploy.sh | grep -v grep
```
*If running, it will output a line with a Process ID (PID) like `1694`.*

### Start the Worker in Background
```bash
nohup bash -c 'while true; do ~/c2go/auto_deploy.sh; sleep 20; done' > ~/c2go/auto_deploy.log 2>&1 &
```

### Stop / Kill the Worker
```bash
pkill -f auto_deploy.sh
```

### Restart the Worker
```bash
pkill -f auto_deploy.sh
nohup bash -c 'while true; do ~/c2go/auto_deploy.sh; sleep 20; done' > ~/c2go/auto_deploy.log 2>&1 &
```

---

## 3. Git & Build Status Commands

### Check Currently Deployed Commit Hash
```bash
cd ~/c2go && git log -1 --format="Commit: %h - %s (%cr) by %an"
```

### Check if Local Machine is in Sync with GitHub
```bash
cd ~/c2go && git fetch origin master --quiet && git status -sb
```
- If in sync: `## master...origin/master`
- If behind: `## master...origin/master [behind 1]`

### Verify Timestamp of the Active Compiler Binary
Check exactly when the binary was last recompiled and deployed:
```bash
ls -lh /usr/local/bin/c2go ~/c2go/c2go
```

### Trigger a Manual Rebuild Immediately
You don't have to wait for the 20-second timer; run the script manually:
```bash
~/c2go/auto_deploy.sh
```

---

## 4. Remote Access & Network Management

### OpenSSH Server Controls
```bash
# Check status:
sudo service ssh status

# Start SSH:
sudo service ssh start

# Stop SSH:
sudo service ssh stop

# Restart SSH:
sudo service ssh restart
```

### Check Active SSH Connections (Who is connected from phone/remote?)
```bash
w
# or:
who
```

### Tailscale Mesh Network Controls
```bash
# View connection status & devices on your private mesh:
tailscale status

# View your static private IPv4:
tailscale ip -4

# Disconnect from Tailscale:
sudo tailscale down

# Reconnect to Tailscale:
sudo tailscale up
```

---

## 5. Testing & Verification

### Test Transpiler & Output via `c2go`
```bash
cd ~/c2go
./c2go transpile examples/hello.c -o output/main.go
go run output/main.go
```

### Test via Instant Mobile Runner (`crun`)
```bash
# Test inline one-liner:
crun 'int main() { printf("Testing live deployment!\n"); return 0; }'
```

---

## 6. Upgrading to a Systemd Service (Auto-Start on Boot)

If you want your auto-deploy worker to automatically start whenever your Debian machine powers on:

### Step 1: Create the Systemd Unit File
```bash
sudo bash -c 'cat << EOF > /etc/systemd/system/c2go-deploy.service
[Unit]
Description=C2Go Self-Hosted Auto Deployment Service
After=network.target

[Service]
Type=simple
User=NidQija
WorkingDirectory=/home/NidQija/c2go
ExecStart=/bin/bash -c "while true; do /home/NidQija/c2go/auto_deploy.sh; sleep 20; done"
Restart=always
RestartSec=5
StandardOutput=append:/home/NidQija/c2go/auto_deploy.log
StandardError=append:/home/NidQija/c2go/auto_deploy.log

[Install]
WantedBy=multi-user.target
EOF'
```

### Step 2: Enable & Start the Service
```bash
sudo systemctl daemon-reload
sudo systemctl enable --now c2go-deploy.service
```

### Step 3: Monitor via Systemctl & Journalctl
```bash
# Check status:
sudo systemctl status c2go-deploy

# View systemd logs:
journalctl -u c2go-deploy -f
```

---

## 7. Emergency & Troubleshooting Cheat Sheet

| Problem | Cause | Solution |
|---|---|---|
| **Termius won't connect** | SSH or Tailscale is stopped | Run `sudo service ssh start` and `sudo tailscale up` |
| **New Git push not deploying** | Worker process died | Run `ps aux \| grep auto_deploy.sh` or restart it |
| **Compiler build error** | C syntax mistake in pushed commit | Run `cat ~/c2go/auto_deploy.log` to view GCC compiler error output |
| **Permission denied on /usr/local/bin/c2go** | Missing sudo permission in script | Ensure user has sudoers permission or run with `sudo` |
| **Log file getting too large** | Weeks of continuous logging | Truncate log with `> ~/c2go/auto_deploy.log` |

