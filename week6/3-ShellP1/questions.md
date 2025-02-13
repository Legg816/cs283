1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() reads input safely by limiting the number of characters read, preventing buffer overflows. It also reads input line by line, making it good for a shell that processes commands sequentially.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() allows dynamic memory allocation, preventing stack overflow for large inputs and enabling flexibility in buffer sizing based on runtime conditions.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Leading and trailing spaces can cause incorrect parsing. This can lead to errors in command execution or unexpected behavior when passing arguments.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  command > output.txt (redirect STDOUT to a file) - Need to handle file I/O operations. command < input.txt (use a file as STDIN) - Must open and read files properly. command 2>&1 (merge STDERR into STDOUT) - Requires proper file descriptor manipulation.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection sends output to files or devices, while piping connects one command's output directly to another command's input.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping error messages separate ensures clearer debugging, preventing errors from mixing with valid output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  It should capture both STDOUT and STDERR, providing an option to merge them using 2>&1 while ensuring error messages are logged separately if needed.