# group-chat-server
## Features

- Accepts multiple client connections using `select()`.
- Broadcasts messages to all connected clients except the sender.
- Allows the server to be shut down with the `\quit` command.

## Prerequisites

- Windows OS
- Visual Studio or another C++ compiler that supports Winsock.
- Basic knowledge of C++ and network programming.

## Setup Instructions

1. **Clone the Repository:**
    ```bash
    git clone https://github.com/yourusername/chat-server.git
    cd chat-server
    ```

2. **Compile the Code:**
    Open the project in Visual Studio or use a command-line compiler. Make sure to link against the Winsock library by adding `ws2_32.lib`.

3. **Run the Server:**
    After compiling, run the server executable. The server will start listening on port `54000` by default.

4. **Connect Clients:**
    Use a TCP client tool or write a client program to connect to the server at the designated IP address and port.

## Usage

- When a new client connects, a welcome message is sent to them, and other clients are notified of the new connection.
- Messages sent by clients are broadcast to all other clients connected to the server.
- The server can be shut down by typing `\quit`.

## Code Explanation

The server uses the `select()` function to manage multiple client connections. The main loop checks for new connections, receives messages from clients, and broadcasts them to all other clients.

### Key Functions:

- **`select()`:** Monitors multiple sockets for readability.
- **`recv()`:** Receives data from a client socket.
- **`send()`:** Sends data to a client socket.
- **`FD_SET` and `FD_CLR`:** Manage the set of socket descriptors.
