#include <iostream>
#include <ws2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")
int main() {
    WSADATA wsadata;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2,2);
    wsaerr = WSAStartup(wVersionRequested, &wsadata);
    if (wsaerr != 0 ){
        std::cout << "The Winsock dll not found!" << std::endl;
        return 1;
    }
    SOCKET serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET){
        std::cout <<"Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }
    //Bind an ip address and port to socket
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(54000);
    service.sin_addr.S_un.S_addr = INADDR_ANY;                                                    //    InetPton(AF_INET, _T(("127.0.0.1")), &service.sin_addr.s_addr);     //converts ip address from string into binary form
    if (bind(serverSocket, (sockaddr*)&service, sizeof(service)) == SOCKET_ERROR){
        std::cout << "Binding has failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    //making it a listening socket
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR){
        std::cout << "listen() error listening on socket" << WSAGetLastError() << std::endl;
        return 1;
    }
    //Creating file descriptor (for sockets) and adding the listening socket
    //so it can hear incoming connections
    fd_set master;
    FD_ZERO(&master);
    FD_SET(serverSocket, &master);

    bool running = true;
    while (running){
        //copy the set because every time we call it we erase the set
        fd_set copy = master;
        int socketCount = select(0,&copy, nullptr, nullptr, nullptr);
        //the only thing a server can do in terms of getting is accept a connection and receive a message

        for (int i = 0; i < socketCount; ++i) {

            //make a temp socket variable and place the socket from the set
            SOCKET socket = copy.fd_array[i];

            char buffer[4096];
            ZeroMemory(buffer, 4096);

            if (socket == serverSocket) {
                //Add the connection to the list of connect sockets by using accept
                //TODO Add more info to the socket so look up what more i can do with the accept function
                SOCKET client = accept(serverSocket, nullptr, nullptr );

                FD_SET(client, &master);
                std::string welcomeMessage = "welcome to the group chat! \r\n";
                send(client, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);

                //TODO: broadcast we have a new connection
                for (int j = 0; j < master.fd_count; ++j) {
                    SOCKET outSock = master.fd_array[j];
                    if (outSock != client && outSock != serverSocket) {
                        std::ostringstream ss;
                        ss << "SOCKET #" << client << " has just joined the chat" << "\r\n";
                        std::string strOut = ss.str();
                        send(outSock, strOut.c_str(), strOut.size()+1, 0);
                    }
                }
            }else {
            //Else it's a message
                int bytesReceived = recv(socket, buffer, 4096, 0 );
                if (bytesReceived <= 0){
                    closesocket(socket);
                    FD_CLR(socket,&master);
                }else {
                    std::string cmd(buffer, bytesReceived);
                    cmd = cmd.substr(0, cmd.find_first_of("\r\n"));  // Trim to remove newline characters

                    std::cout << "Received command: " << cmd << std::endl;  // Debugging output

                    if (cmd == "\\quit") {
                        std::cout << "Shutting down server..." << std::endl;
                        running = false;
                        break;
                    }

                    // else send a message to the other clients and not the listening socket
                    for (int j = 0; j < master.fd_count; ++j) {
                        SOCKET outSock = master.fd_array[j];
                        if (outSock != socket && outSock != serverSocket){
                            std::ostringstream ss;
                            ss << "SOCKET #" << socket << ": " << buffer << "\r\n";
                            std::string strOut = ss.str();

                            send(outSock, strOut.c_str(),strOut.size() + 1 ,0);
                        }
                    }
                }
            }
        }
    }
    FD_CLR(serverSocket, &master);
    closesocket(serverSocket);

    // Message to let users know what's happening.
    std::string msg = "Server is shutting down. Goodbye\r\n";
    while (master.fd_count > 0) {
        // Get the socket number
        SOCKET sock = master.fd_array[0];

        // Send the goodbye message
        send(sock, msg.c_str(), msg.size(), 0);

        // Remove it from the master file list and close the socket
        FD_CLR(sock, &master);
        closesocket(sock);
    }

    WSACleanup();
    return 0;
}
