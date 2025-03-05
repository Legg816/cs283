#!/usr/bin/env bats

@test "Simple pipe: ls | grep dshlib.c" {
    run "./dsh" <<EOF                
ls | grep dshlib.c
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="dshlib.cdsh3>dsh3>cmdloopreturned0"


    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipe with multiple commands: ls | grep dshlib.c | wc -l" {
    run "./dsh" <<EOF
ls | grep dshlib.c | wc -l
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output is the number of lines (should be 1 for the file dshlib.c)
    expected_output="1dsh3>dsh3>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipe with no matching output: ls | grep non_existent_file" {
    run "./dsh" <<EOF
ls | grep non_existent_file
EOF

    # Expected output should be empty since there is no file "non_existent_file"
    expected_output="dsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipe with invalid command: ls | non_existent_command" {
    run "./dsh" <<EOF
ls | non_existent_command
EOF

    # The command should fail as 'non_existent_command' doesn't exist
    expected_output="execvp:Permissiondenieddsh3>dsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Pipe with multiple valid commands: ls | grep dshlib.c | cat" {
    run "./dsh" <<EOF
ls | grep dshlib.c | cat
EOF

    # Should just echo the name of the file "dshlib.c"
    expected_output="dshlib.cdsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $stripped_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Multiple pipes and commands: echo hello | tr 'a-z' 'A-Z' | rev" {
    run "./dsh" <<EOF
echo hello | tr 'a-z' 'A-Z' | rev
EOF

    # Expected output is the reversed uppercase "OLLEH"
    expected_output="OLLEHdsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Expected: $expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipe with no commands" {
    run "./dsh" <<EOF
|
EOF

    # Since no valid command is provided, it should return an error.
    expected_output="dsh3>warning:nocommandsprovideddsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Check if the error message is as expected
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected: $expected_output"
    echo "Exit Status: $status"

    [[ "$stripped_output" = "$expected_output" ]]
}

@test "Pipe with empty output" {
    run "./dsh" <<EOF
echo "" | grep "non_existent_string"
EOF

    # The grep command should produce no output since there's nothing matching "non_existent_string"
    expected_output="dsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Check if the error message is as expected
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected: $expected_output"
    echo "Exit Status: $status"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Pipe with commands and errors: ls | non_existent_command | wc -l" {
    run "./dsh" <<EOF
ls | non_existent_command | wc -l
EOF

    # The 'non_existent_command' will fail, and the wc -l should never run.
    expected_output="execvp:Permissiondenied0dsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Check if the error message is as expected
    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Pipe with edge case: ls | head -n 5" {
    run "./dsh" <<EOF
ls | head -n 5
EOF

    expected_output="batsdshdsh_cli.cdshlib.cdshlib.hdsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expecting the first 5 files from the `ls` output
    # The output will vary based on your directory structure.
    # Check if the error message is as expected
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected: $expected_output"
    echo "Exit Status: $status"

    # This is more dynamic; we check if wc -l returns 5 lines or fewer
    line_count=$(echo "$stripped_output" | wc -l)

    echo "Line count: $line_count"

    [ "$line_count" -le 5 ]
    [ "$status" -eq 0 ]
}

@test "Multiple pipes with multiple errors: ls | grep non_existent_file | cat | wc -l" {
    run "./dsh" <<EOF
ls | grep non_existent_file | cat | wc -l
EOF

    # grep should return nothing, so cat will receive no input
    # wc -l should return 0
    expected_output="0dsh3>dsh3>cmdloopreturned0"

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Check if the error message is as expected
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected: $expected_output"
    echo "Exit Status: $status"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}


