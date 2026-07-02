#!/bin/bash
# CASM Installer
# Installs the CASM assembler + VM to your system
# Usage: bash install.sh

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

print_header() {
    echo -e "\n${BLUE}╔════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║${NC}  CASM Assembler + VM Installer   ${BLUE}║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════╝${NC}\n"
}

print_step() {
    echo -e "${GREEN}[*]${NC} $1"
}

print_error() {
    echo -e "${RED}[!]${NC} $1" >&2
}

print_success() {
    echo -e "${GREEN}[✓]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[⚠]${NC} $1"
}

print_header

# Determine install location
if [ "$EUID" -eq 0 ]; then
    INSTALL_PREFIX="/opt/casm"
    LAUNCHER_DIR="/usr/local/bin"
    SYSTEM_WIDE=true
else
    INSTALL_PREFIX="$HOME/.local/casm"
    LAUNCHER_DIR="$HOME/.local/bin"
    SYSTEM_WIDE=false
fi

echo "Installation prefix:  $INSTALL_PREFIX"
echo "Launcher directory:   $LAUNCHER_DIR"
if [ "$SYSTEM_WIDE" = true ]; then
    echo "Mode:                 System-wide (root)"
else
    echo "Mode:                 User local"
fi
echo ""

# Check prerequisites
print_step "Checking prerequisites..."
if ! command -v gcc &> /dev/null; then
    print_error "gcc not found. Please install build-essential:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  Fedora/RHEL:   sudo yum install gcc"
    echo "  macOS:         xcode-select --install"
    exit 1
fi
print_success "gcc found: $(gcc --version | head -1)"

if ! command -v python3 &> /dev/null; then
    print_error "python3 not found. Please install it."
    exit 1
fi
print_success "python3 found: $(python3 --version)"

# Check if we're in the CASM source directory
print_step "Verifying CASM source files..."
if [ ! -f "source/Language.py" ] || [ ! -f "source/ByteCode.py" ] || [ ! -f "source/Cpu.c" ]; then
    print_error "Source files not found!"
    echo ""
    echo "This script must be run from the CASM repository root."
    echo "Expected files:"
    echo "  source/Language.py"
    echo "  source/ByteCode.py"
    echo "  source/Cpu.c"
    echo "  source/Instructions.c"
    echo "  source/Tools.c"
    echo "  include/Instructions.h"
    echo "  include/Errors.h"
    echo "  include/Tools.h"
    exit 1
fi
print_success "Source files found"
echo ""

# Create directories
print_step "Creating directories..."
mkdir -p "$INSTALL_PREFIX/casm_home/source"
mkdir -p "$INSTALL_PREFIX/casm_home/include"
mkdir -p "$LAUNCHER_DIR"
print_success "Directories created"

# Copy source files
print_step "Copying source files..."
cp source/Language.py "$INSTALL_PREFIX/casm_home/source/"
cp source/ByteCode.py "$INSTALL_PREFIX/casm_home/source/"
cp source/Cpu.c "$INSTALL_PREFIX/casm_home/source/"
cp source/Instructions.c "$INSTALL_PREFIX/casm_home/source/"
cp source/Tools.c "$INSTALL_PREFIX/casm_home/source/"
cp include/Errors.h "$INSTALL_PREFIX/casm_home/include/"
cp include/Instructions.h "$INSTALL_PREFIX/casm_home/include/"
cp include/Tools.h "$INSTALL_PREFIX/casm_home/include/"
print_success "Source files copied"

# Copy examples if they exist
if [ -d "examples" ]; then
    print_step "Copying examples..."
    mkdir -p "$INSTALL_PREFIX/examples"
    cp examples/*.casm "$INSTALL_PREFIX/examples/" 2>/dev/null || true
    print_success "Examples copied"
fi

# Compile the VM
print_step "Compiling VM binary..."
cd "$INSTALL_PREFIX/casm_home"
if gcc $(find ./source -name "*.c") -o ../casm_vm 2> /tmp/gcc_error.log; then
    chmod +x ../casm_vm
    VM_SIZE=$(stat -c%s "../casm_vm" 2>/dev/null || stat -f%z "../casm_vm" 2>/dev/null || echo "?")
    print_success "VM compiled: ../casm_vm ($VM_SIZE bytes)"
else
    print_error "Compilation failed!"
    echo ""
    echo "GCC output:"
    cat /tmp/gcc_error.log
    exit 1
fi
cd - > /dev/null

# Install launcher
print_step "Installing launcher script..."
cat > "$LAUNCHER_DIR/casm" << 'LAUNCHER_SCRIPT'
#!/bin/bash
# CASM Launcher
# Automatically finds the installation and runs your CASM program

if [ $# -lt 1 ]; then
    echo "Usage: casm <input.casm>"
    echo ""
    echo "Assembles and executes a CASM program."
    echo ""
    echo "Examples:"
    echo "  casm program.casm"
    echo "  casm examples/01_factorial.casm"
    exit 1
fi

INPUT_FILE="$1"

# Verify file exists
if [ ! -f "$INPUT_FILE" ]; then
    echo "[ERROR] File not found: $INPUT_FILE" >&2
    exit 1
fi

# Find CASM installation
CASM_HOME=""
if [ -x "$HOME/.local/casm/casm_vm" ]; then
    CASM_HOME="$HOME/.local/casm"
elif [ -x "/opt/casm/casm_vm" ]; then
    CASM_HOME="/opt/casm"
else
    # Try to find relative to this script
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." 2>/dev/null && pwd)" || SCRIPT_DIR=""
    if [ -x "$SCRIPT_DIR/casm/casm_vm" ]; then
        CASM_HOME="$SCRIPT_DIR/casm"
    fi
fi

if [ -z "$CASM_HOME" ] || [ ! -x "$CASM_HOME/casm_vm" ]; then
    echo "[ERROR] CASM installation not found" >&2
    echo "        Expected locations:" >&2
    echo "          ~/.local/casm/casm_vm" >&2
    echo "          /opt/casm/casm_vm" >&2
    exit 1
fi

# Create temporary directory for this run
TEMP_DIR=$(mktemp -d) || { echo "[ERROR] Failed to create temp directory" >&2; exit 1; }
trap "rm -rf $TEMP_DIR" EXIT

# Step 1: Lex and parse the input file
if ! python3 "$CASM_HOME/casm_home/source/Language.py" "$INPUT_FILE" > "$TEMP_DIR/parsed.json" 2>&1; then
    echo "[ERROR] Parsing failed:" >&2
    head -20 "$TEMP_DIR/parsed.json" >&2
    exit 1
fi

# Step 2: Assemble to bytecode
if ! python3 "$CASM_HOME/casm_home/source/ByteCode.py" < "$TEMP_DIR/parsed.json" > "$TEMP_DIR/program.bin" 2>&1; then
    echo "[ERROR] Assembly failed:" >&2
    head -20 "$TEMP_DIR/program.bin" >&2
    exit 1
fi

# Step 3: Execute on the VM
"$CASM_HOME/casm_vm" "$TEMP_DIR/program.bin"
exit $?
LAUNCHER_SCRIPT

chmod +x "$LAUNCHER_DIR/casm"
print_success "Launcher installed: $LAUNCHER_DIR/casm"

# Check PATH
echo ""
print_step "Verifying PATH configuration..."
if [[ ":$PATH:" == *":$LAUNCHER_DIR:"* ]]; then
    print_success "$LAUNCHER_DIR is in PATH"
else
    print_warning "$LAUNCHER_DIR is NOT in your PATH"
    echo ""
    if [ "$SYSTEM_WIDE" = false ]; then
        echo "Add this line to ~/.bashrc or ~/.zshrc:"
        echo ""
        echo "    export PATH=\"$HOME/.local/bin:\$PATH\""
        echo ""
        echo "Then reload your shell:"
        echo "    source ~/.bashrc"
    fi
fi

echo ""
print_success "Installation complete!"
echo ""
echo "Next steps:"
echo "  1. Source your shell config if PATH was updated"
echo "  2. Try it out: ${BLUE}casm examples/01_factorial.casm${NC}"
echo ""
echo "For more info, see: INSTALL.md"
echo ""
