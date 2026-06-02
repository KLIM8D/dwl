#!/bin/bash

# DWL Architecture-Aware Patch Manager
set -e

# Configuration
PATCHES_DIR="dwl-patches-repo" # Local clone of the patches repo
UPSTREAM_REMOTE="https://github.com/djpohly/dwl.git"
PATCH_REMOTE="https://codeberg.org/dwl/dwl-patches.git"
PROGRESS_FILE=".git/PM_INSTALL_IN_PROGRESS"

# Colors
RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'; BLUE='\033[0;34m'; NC='\033[0m'
log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Ensure we are in a git repo and ignore the patches dir locally
init_check() {
    if [ ! -d .git ]; then log_error "Not a git repository"; exit 1; fi
    if ! grep -q "$PATCHES_DIR" .git/info/exclude 2>/dev/null; then
        echo "$PATCHES_DIR" >> .git/info/exclude
    fi
}

show_help() {
    cat << EOF
DWL Management:
  setup         - Initialize the 3-branch structure (Run this first!)
  sync          - Update upstream and rebase patches branch
  browse        - Show available patches
  install <p>   - Apply patch as a new commit on 'patches' branch
  build         - Update 'master' from 'patches' and compile
  list          - List applied patches
EOF
}

setup_branches() {
    log_info "Initializing 3-branch architecture..."
    
    # 1. Setup Upstream Remote
    if ! git remote | grep -q "upstream"; then
        git remote add upstream "$UPSTREAM_REMOTE"
    fi
    git fetch upstream main --tags

    # 2. Force 'upstream' branch to be a clean mirror
    log_info "Creating clean upstream branch..."
    git checkout -B upstream
    git reset --hard v0.8
    
    # 3. Create 'patches' from 'upstream'
    log_info "Creating patches branch..."
    git checkout -B patches upstream
    
    # 4. Create 'master' from 'patches'
    log_info "Creating master build branch..."
    git checkout -B main patches
    
    log_success "Architecture ready: upstream (clean) -> patches -> master"
    
    # Clone patches repo if missing
    if [ ! -d "$PATCHES_DIR" ]; then
        log_info "Cloning official patches repository..."
        git clone "$PATCH_REMOTE" "$PATCHES_DIR"
    fi
}

sync_upstream() {
    log_info "Fetching latest dwl source..."
    git checkout upstream
    git pull upstream main
    
    log_info "Rebasing patches onto new upstream..."
    git checkout patches
    if git rebase upstream; then
        log_success "Patches rebased successfully."
    else
        log_error "Rebase conflict! Fix manually, then run 'git rebase --continue'"
        exit 1
    fi
}

install_patch() {
    local name=$1
    [ -z "$name" ] && { log_error "Patch name required"; exit 1; }
    
    git checkout patches
    local patch_dir="$PATCHES_DIR/patches/$name"
    
    if [ ! -d "$patch_dir" ]; then
        log_error "Patch directory '$name' not found."; exit 1
    fi

    # 1. Collect all .patch files
    mapfile -t files < <(find "$patch_dir" -maxdepth 1 -name "*.patch" | sort)
    [ ${#files[@]} -eq 0 ] && { log_error "No patches found."; exit 1; }

    # 2. Display the menu
    log_info "Available files for '$name':"
    for i in "${!files[@]}"; do
        echo "  $((i+1))) ${files[$i]##*/}"
    done
    echo "  c) Cancel"
    echo ""
    read -p "Enter numbers to apply (space-separated, e.g. '1 3'): " choices

    if [[ "$choices" == "c" ]]; then log_info "Cancelled."; return; fi

    # 3. Apply selected files in order
    for choice in $choices; do
        # Convert 1-based index to 0-based
        idx=$((choice-1))
        p_file="${files[$idx]}"

        if [ -f "$p_file" ]; then
            log_info "Applying: ${p_file##*/}"
            
            # Use 3-way to allow interactive resolution if it fails
            if git apply --3way "$p_file"; then
                git add .
                git commit -m "feat(patch): $name (${p_file##*/})"
                log_success "Applied ${p_file##*/}"
            else
                # If a file fails, we MUST stop to let the user fix it
                echo "$name" > "$PROGRESS_FILE"
                log_warn "CONFLICT in ${p_file##*/}. Resolve and run './pm.sh continue'."
                log_info "Note: Remaining patches in your list were skipped. Apply them after continuing."
                exit 1
            fi
        else
            log_error "Invalid selection: $choice"
        fi
    done
}

continue_op() {
    # Check if a standard Git rebase is in progress
    if [ -d .git/rebase-merge ] || [ -d .git/rebase-apply ]; then
        log_info "Continuing Git rebase..."
        git rebase --continue
    # Check if our custom patch install is in progress
    elif [ -f "$PROGRESS_FILE" ]; then
        local name=$(cat "$PROGRESS_FILE")
        log_info "Finishing installation of $name..."
        git add .
        git commit -m "feat(patch): $name"
        rm "$PROGRESS_FILE"
        log_success "Patch $name successfully committed."
    else
        log_error "No operation in progress to continue."
    fi
}

build_master() {
    log_info "Updating master branch..."
    git checkout main 
    git merge patches --no-edit
    
    log_info "Compiling..."
    make && log_success "Build complete."
}

list_patches() {
    log_info "Applied Patches (Commits on 'patches' not in 'upstream'):"
    git log upstream..patches --oneline --color | sed 's/^/  /'
    
    echo ""
    log_info "Sync Status:"
    BEHIND=$(git rev-list --count patches..upstream)
    AHEAD=$(git rev-list --count upstream..patches)
    echo "  - Your patch stack is $AHEAD patches high."
    echo "  - You are $BEHIND commits behind upstream."
}

version() {
    VERSION=$1
    [ -z "$VERSION" ] && { log_error "Usage: ./pm.sh version v0.8"; exit 1; }
    
    log_info "Switching base to $VERSION..."
    git checkout upstream
    git fetch upstream --tags
    
    if git reset --hard "$VERSION"; then
        log_info "Rebasing patches onto $VERSION..."
        git checkout patches
        if git rebase upstream; then
            log_success "Successfully moved patch stack to $VERSION"
        else
            log_error "Rebase failed. Fix conflicts manually then run 'git rebase --continue'"
        fi
    else
        log_error "Version $VERSION not found."
    fi
}

uninstall() {
    log_info "Opening interactive rebase to manage patches..."
    log_warn "Change 'pick' to 'drop' for the patch you want to remove."
    git checkout patches
    git rebase -i upstream
}

case "${1:-help}" in
    setup) setup_branches ;;
    sync)  sync_upstream ;;
    browse) ls -1 "$PATCHES_DIR/patches/" ;;
    install) install_patch "$2" ;;
    continue) continue_op ;;
    build) build_master ;;
    list) list_patches ;;
    version) version "$2" ;;
    uninstall) uninstall ;;
    *) show_help ;;
esac
