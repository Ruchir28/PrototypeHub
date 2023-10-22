#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <vector>
#include "server.h"

class LoadBalancer {
public:
    LoadBalancer(int port);
    void addServer(Server server);
    void start();
    void handleClientRequest(int client_socket);

private:
    std::vector<Server> servers;
    int currentServerIndex;
    int port;
    int lb_fd;  // File descriptor for the LoadBalancer's own listening socket
    sockaddr_in lb_address;  // Address for the LoadBalancer's own socket
    int connectToServer(const Server& server);
};

#endif // LOAD_BALANCER_H