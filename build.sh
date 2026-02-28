#!/bin/bash

# LOG4CPP Build Script
# Builds both static and dynamic libraries

set -e

echo "=== LOG4CPP Build System ==="
echo ""

# Configuration
COMPILER="g++"
CPPFLAGS="-std=c++14 -Wall -Wextra -fPIC"
OPTIMIZATION="-O2"
INCLUDE_DIR="./Includes"
SRC_DIR="./Src"
LIB_DIR="./lib"
INSTALL_DIR="${INSTALL_DIR:-.}"

# Library names
STATIC_LIB="liblog4cpp.a"
DYNAMIC_LIB="liblog4cpp.so"

# Source files
SOURCES=(
    "$SRC_DIR/Logger.cpp"
    "$SRC_DIR/Logger_C.cpp"
)

# Create lib directory
mkdir -p "$LIB_DIR"

# Temporary directory for object files
TMP_BUILD_DIR=$(mktemp -d)
trap "rm -rf $TMP_BUILD_DIR" EXIT

echo "Configuration:"
echo "  Compiler: $COMPILER"
echo "  C++ Standard: C++14"
echo "  Optimization: $OPTIMIZATION"
echo "  Include Dir: $INCLUDE_DIR"
echo "  Library Dir: $LIB_DIR"
echo ""

# Compile source files to temporary object files
echo "Compiling source files..."
OBJECTS=()
for src in "${SOURCES[@]}"; do
    obj_name=$(basename "$src" .cpp).o
    obj="$TMP_BUILD_DIR/$obj_name"
    OBJECTS+=("$obj")
    echo "  Compiling: $src"
    $COMPILER $CPPFLAGS $OPTIMIZATION -I"$INCLUDE_DIR" -c "$src" -o "$obj"
done
echo "✓ Compilation complete"
echo ""

# Create static library
echo "Creating static library: $STATIC_LIB"
ar rcs "$LIB_DIR/$STATIC_LIB" "${OBJECTS[@]}"
echo "✓ Static library created"
echo ""

# Create dynamic library
echo "Creating dynamic library: $DYNAMIC_LIB"
$COMPILER -shared -fPIC $CPPFLAGS $OPTIMIZATION -I"$INCLUDE_DIR" "${SOURCES[@]}" -o "$LIB_DIR/$DYNAMIC_LIB"
echo "✓ Dynamic library created"
echo ""

# Display built libraries
echo "=== Build Output ==="
ls -lh "$LIB_DIR"/$STATIC_LIB "$LIB_DIR"/$DYNAMIC_LIB
echo ""

echo "Library Information:"
echo "  Static:  $LIB_DIR/$STATIC_LIB"
echo "  Dynamic: $LIB_DIR/$DYNAMIC_LIB"
echo ""

echo "=== Build Complete ==="
echo ""
echo "To use these libraries:"
echo "  C++: -I./Includes -L./lib -llog4cpp"
echo "  C:   -I./Includes/Logger_C.h -L./lib -llog4cpp"
echo ""
echo "For dynamic linking, set: export LD_LIBRARY_PATH=./lib:\$LD_LIBRARY_PATH"
