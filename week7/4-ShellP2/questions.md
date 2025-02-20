1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  fork creates a new child process, allowing the parent process to continue running independently. If we called execvp directly in the parent, it would replace the parent process, terminating our shell.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it usually indicates a system resource issue, such as running out of process slots. My implementation handles this by printing an error message and returning without executing the command.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in directories listed in the PATH environment variable, checking each directory until it finds an executable match.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() ensures the parent process waits for the child to complete, preventing zombie processes. Without wait(), completed child processes would remain in a "defunct" state, consuming system resources.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() extracts the exit status of a child process from the wait() status, allowing the shell to determine whether the command succeeded or failed.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  It preserves spaces within quoted arguments by treating them as a single token instead of splitting them at spaces, ensuring commands like echo "hello     world    " work correctly and preserves spaces.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I improved handling of quoted arguments and whitespace, making parsing more robust. The biggest challenge was ensuring proper tokenization while maintaining command structure.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals provide a way for processes to communicate asynchronously, handling events like termination or suspension without direct function calls or shared memory.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (9): Immediately terminates a process and cannot be caught or ignored. SIGTERM (15): Politely asks a process to terminate, allowing cleanup. SIGINT (2): Sent when a user presses Ctrl+C to interrupt a process.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP pauses a process but cannot be caught or ignored, ensuring the process is suspended until resumed with SIGCONT.
