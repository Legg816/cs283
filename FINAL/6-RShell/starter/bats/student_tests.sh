#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

setup() {
    TEST_DIR="$BATS_TEST_DIRNAME/.."
    
    # Kill any running instances of dsh
    pkill -f "./dsh -s" || true

    # Start the server in the background
    (cd "$TEST_DIR" && ./dsh -s > server.log 2>&1 &)
    SERVER_PID=$!

    # Give the server time to start
    sleep 2

    echo "Server started successfully with PID: $SERVER_PID"
}

# 🛠️ Cleanup: Kill the server after tests
teardown() {
    kill $SERVER_PID 2>/dev/null || true
}

@test "Basic Command Execution - ls" {
    run "./dsh -c" <<EOF
ls
exit
EOF

    echo "Captured stdout: $output"
    echo "Exit Status: $status"

    [[ "$output" =~ "dsh4>" ]]
    [[ "$output" =~ "dsh" ]]
    [ "$status" -eq 0 ]
}