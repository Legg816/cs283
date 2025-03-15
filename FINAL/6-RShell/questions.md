1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines the end of a command's output by looking for a special end-of-transmission marker, such as the EOF character sent by the server. To handle partial reads, the client should use a looped recv() call to continuously read data until the expected delimiter is received. Buffering techniques and non-blocking I/O can also help ensure that message transmission is complete.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should define explicit message boundaries using a special delimiter (e.g., a null byte for commands and EOF for output termination). Without this, the client may split or merge messages incorrectly, leading to truncated or combined outputs, making it impossible to distinguish where one command ends and another begins.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol remembers past interactions, meaning the server maintains session information between requests (e.g., TCP, FTP, HTTP sessions). A stateless protocol, on the other hand, treats each request independently, with no memory of previous interactions (e.g., HTTP, UDP). Stateless protocols scale better but may require client-side state tracking.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is used when low latency and real-time data transmission are more important than guaranteed delivery, such as in video streaming, online gaming, and VoIP. Unlike TCP, UDP has no retransmissions, lower overhead, and faster delivery, making it a good choice for applications that can tolerate some packet loss.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Operating systems provide the Sockets API, which allows applications to open, read from, write to, and close network connections using system calls like socket(), connect(), send(), and recv(). This abstraction works across various network protocols (TCP, UDP) and allows applications to communicate over the internet.