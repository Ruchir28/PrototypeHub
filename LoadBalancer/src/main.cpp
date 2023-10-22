#include "server.h"
#include "load_balancer.h"
#include <thread>      // for std::thread
#include <functional>  // for std::ref
#include <unistd.h>    // for sleep


int main() {
    // Create a couple of servers
    Server server1("Server1", 8081);
    Server server2("Server2", 8082);

    // Initialize them
    server1.setup();
    server2.setup();

    std::thread t1(&Server::listenForRequests, std::ref(server1));
    std::thread t2(&Server::listenForRequests, std::ref(server2));

    sleep(5);
    // Create the LoadBalancer
    LoadBalancer lb(8080);

    // Add servers to LoadBalancer
    lb.addServer(server1);
    lb.addServer(server2);

    // Start the LoadBalancer
    lb.start();

    return 0;
}
