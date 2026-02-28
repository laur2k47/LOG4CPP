#!/bin/bash

set -e

echo "================================"
echo "Building all tests..."
echo "================================"
./build_tests.sh
g++ -std=c++14 -O2 -I../Includes test_multiple_handlers.cpp -o build/test_multi -L../lib -llog4cpp 2>/dev/null

export LD_LIBRARY_PATH=../lib:$LD_LIBRARY_PATH

echo ""
echo "================================"
echo "1. C Static Linking Test"
echo "================================"
./build/test_c_static | head -20

echo ""
echo "================================"
echo "2. C Dynamic Linking Test"
echo "================================"
./build/test_c_dynamic | head -20

echo ""
echo "================================"
echo "3. C++ Static Linking Test"
echo "================================"
./build/test_cpp_static | head -20

echo ""
echo "================================"
echo "4. C++ Dynamic Linking Test"
echo "================================"
./build/test_cpp_dynamic | head -20

echo ""
echo "================================"
echo "5. Multiple Handlers Test"
echo "================================"
./build/test_multi

echo ""
echo "✓ All tests completed!"
