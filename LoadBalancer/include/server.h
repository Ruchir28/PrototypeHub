#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class Server {
public:
    Server(std::string id, int port);
    void setup();
    void listenForRequests();
    void handleRequest(int socket);
    int getPort() const;
    std::string getId() const;
    
private:
    std::string id;
    int port;
    int server_fd;
    sockaddr_in address;
};

#endif // SERVER_H
