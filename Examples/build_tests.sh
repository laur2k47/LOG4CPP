#!/bin/bash

# Test Applications Build Script
# Builds C and C++ test apps with static and dynamic libraries

set -e

COMPILER_C="gcc"
COMPILER_CPP="g++"
CPPFLAGS="-std=c++14 -Wall -Wextra"
INCLUDE_DIR="../Includes"
LIB_DIR="../lib"
BUILD_DIR="./build"

echo "=== LOG4CPP Test Applications Build ==="
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"

# Function to build target
build_target() {
    local name="$1"
    local source="$2"
    local compiler="$3"
    local lib_type="$4"  # static or dynamic
    
    local output="$BUILD_DIR/$name"
    
    if [ "$lib_type" = "static" ]; then
        echo "Building: $name (static linking)"
        if [ "$compiler" = "$COMPILER_C" ]; then
            $compiler -I"$INCLUDE_DIR" "$source" "$LIB_DIR/liblog4cpp.a" -lstdc++ -o "$output"
        else
            $compiler $CPPFLAGS -I"$INCLUDE_DIR" "$source" "$LIB_DIR/liblog4cpp.a" -o "$output"
        fi
    else
        echo "Building: $name (dynamic linking)"
        if [ "$compiler" = "$COMPILER_C" ]; then
            $compiler -I"$INCLUDE_DIR" "$source" -L"$LIB_DIR" -llog4cpp -lstdc++ -o "$output"
        else
            $compiler $CPPFLAGS -I"$INCLUDE_DIR" "$source" -L"$LIB_DIR" -llog4cpp -o "$output"
        fi
    fi
    echo "  ✓ Created: $output"
}

echo "Configuration:"
echo "  Include Dir: $INCLUDE_DIR"
echo "  Library Dir: $LIB_DIR"
echo "  Build Dir: $BUILD_DIR"
echo ""

# Check if libraries exist
if [ ! -f "$LIB_DIR/liblog4cpp.a" ] || [ ! -f "$LIB_DIR/liblog4cpp.so" ]; then
    echo "Error: Libraries not found in $LIB_DIR"
    echo "Please build the libraries first using: ../build.sh"
    exit 1
fi

echo "Building test applications..."
echo ""

# Build targets
build_target "test_c_static"   "test_c.c"     "$COMPILER_C"   "static"
build_target "test_c_dynamic"  "test_c.c"     "$COMPILER_C"   "dynamic"
build_target "test_cpp_static" "test_cpp.cpp" "$COMPILER_CPP" "static"
build_target "test_cpp_dynamic" "test_cpp.cpp" "$COMPILER_CPP" "dynamic"

# Build rotation test (C++14 required for std::bind and std::filesystem)
echo "Building: test_rotation (static linking with file rotation)"
$COMPILER_CPP $CPPFLAGS -I"$INCLUDE_DIR" "test_rotation.cpp" "$LIB_DIR/liblog4cpp.a" -o "$BUILD_DIR/test_rotation"
echo "  ✓ Created: $BUILD_DIR/test_rotation"

echo ""
echo "=== Build Complete ==="
echo ""
echo "Run tests:"
echo "  Static linking:"
echo "    $BUILD_DIR/test_c_static"
echo "    $BUILD_DIR/test_cpp_static"
echo ""
echo "  Dynamic linking:"
echo "    export LD_LIBRARY_PATH=$LIB_DIR:\$LD_LIBRARY_PATH"
echo "    $BUILD_DIR/test_c_dynamic"
echo "    $BUILD_DIR/test_cpp_dynamic"
echo ""
