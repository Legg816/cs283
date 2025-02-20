#!/usr/bin/env bats

@test "Exit command" {
    run "./dsh" <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Echo command" {
    run "./dsh" <<EOF
echo Hello World
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="HelloWorlddsh2>dsh2>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
}

@test "List directory contents" {
    run "./dsh" <<EOF
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Change to home directory" {
    run "./dsh" <<EOF
cd ~
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "Attempt to cd into a non-existent directory" {
    run "./dsh" <<EOF
cd /nonexistentdir
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="cd:Nosuchfileordirectorydsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

}

@test "Check current directory after cd" {
    mkdir -p testdir
    run "./dsh" <<EOF
cd testdir
pwd
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="$(pwd)/testdirdsh2>dsh2>dsh2>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"


    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    rmdir testdir
}

@test "Run a command with arguments" {
    run "./dsh" <<EOF
grep root /etc/passwd
EOF
    [ "$status" -eq 0 ]
}

@test "Handle command not found" {
    run "./dsh" <<EOF
nonexistentcommand
EOF
    [ "$status" -ne 250 ]
}

@test "Ensure background process runs" {
    run "./dsh" <<EOF
sleep 1 &
EOF
    [ "$status" -eq 0 ]
}

@test "Check quoted arguments are preserved" {
    run "./dsh" <<EOF
echo "Hello    world"
EOF
    stripped_output=$(echo "$output" | tr -d '	

')
    expected_output="Hello    worlddsh2> dsh2> cmd loop returned 0"
    [ "$stripped_output" = "$expected_output" ]
}

@test "Verify handling of empty input" {
    run "./dsh" <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "Ensure environment variables expand" {
    run "./dsh" <<EOF
echo $HOME
EOF
    [ "$status" -eq 0 ]
}