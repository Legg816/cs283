1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

By calling waitpid() for each child process, the shell ensures that it waits for all child processes to finish before returning to the prompt. If waitpid() is not called, zombie processes would accumulate, leading to resource leaks and potential process table overflow.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

Closing unused pipe ends prevents deadlocks by allowing EOF to be detected properly. Leaving pipes open can cause child processes to hang indefinitely, waiting for input when none will arrive. 

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command needs to change the current working directory of the shell process itself, which is not possible if executed as a separate process. An external cd would only affect its own process and terminate without impacting the shell's directory.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

Using dynamic memory allocation for pipe and process management arrays would allow handling an arbitrary number of commands. However, this increases complexity and requires careful memory management to avoid leaks and ensure efficiency.
