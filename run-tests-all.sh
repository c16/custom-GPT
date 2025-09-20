#!/bin/bash
# Comprehensive Test Runner for Claude Agent
# Runs both Python and C++ tests

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Claude Agent Comprehensive Test Suite"
echo "====================================="
echo "Running all tests for both Python and C++ implementations"
echo ""

TOTAL_FAILURES=0

# Run Python tests
echo "🐍 PYTHON TESTS"
echo "==============="
if bash "$SCRIPT_DIR/run-tests-python.sh"; then
    echo "✅ Python tests completed successfully"
else
    echo "❌ Python tests failed"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

echo ""
echo ""

# Run C++ tests
echo "⚙️  C++ TESTS"
echo "============="
if bash "$SCRIPT_DIR/run-tests-cpp.sh"; then
    echo "✅ C++ tests completed successfully"
else
    echo "❌ C++ tests failed"
    TOTAL_FAILURES=$((TOTAL_FAILURES + 1))
fi

echo ""
echo ""

# Final summary
echo "🏁 FINAL TEST SUMMARY"
echo "===================="
if [ $TOTAL_FAILURES -eq 0 ]; then
    echo "🎉 ALL TESTS PASSED!"
    echo "Both Python and C++ implementations are working correctly."
    echo ""
    echo "✅ Python implementation: PASSED"
    echo "✅ C++ implementation: PASSED"
    echo ""
    echo "The custom agent application is ready for use!"
    exit 0
else
    echo "💥 SOME TESTS FAILED!"
    echo "Number of test suites that failed: $TOTAL_FAILURES"
    echo ""

    if bash "$SCRIPT_DIR/run-tests-python.sh" > /dev/null 2>&1; then
        echo "✅ Python implementation: PASSED"
    else
        echo "❌ Python implementation: FAILED"
    fi

    if bash "$SCRIPT_DIR/run-tests-cpp.sh" > /dev/null 2>&1; then
        echo "✅ C++ implementation: PASSED"
    else
        echo "❌ C++ implementation: FAILED"
    fi

    echo ""
    echo "Please review the test output above and fix any issues."
    exit 1
fi