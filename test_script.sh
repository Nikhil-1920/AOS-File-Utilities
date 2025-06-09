#!/bin/bash

echo "🚀 Testing CS3304 Assignment 1"
echo "================================"

# Compile programs
echo "📦 Compiling programs..."
g++ -o q1 2024201067_A1_Q1.cpp
g++ -o q2 2024201067_A1_Q2.cpp

# Create test files
echo "📝 Creating test files..."
echo "Hello, World!" > test.txt
echo "The quick brown fox jumps over the lazy dog" > long_test.txt

# Test Q1 - Complete reversal
echo "🔄 Testing complete file reversal..."
./q1 test.txt 0

# Test Q1 - Partial reversal
echo "🔄 Testing partial file reversal..."
./q1 long_test.txt 1 4 8

# Test Q2 - Validation
echo "🔍 Testing file validation..."
./q2 Assignment1/0_test.txt test.txt Assignment1

echo "✅ All tests completed!"
