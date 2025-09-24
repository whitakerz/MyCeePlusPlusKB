#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdint>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int32_t payload[2] = {0};
    int32_t sum = 0;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation error!" << std::endl;
        return -1;
    }

    // Assigning address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Binding error!" << std::endl;
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listening error!" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port: " << PORT << std::endl;

    // Accept the connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        std::cerr << "Accepting connection failed!" << std::endl;
        return -1;
    }

    // Read two integers from client
    ssize_t total_received = 0;
    char* payload_bytes = reinterpret_cast<char*>(payload);
    while (total_received < static_cast<ssize_t>(sizeof(payload))) {
        ssize_t received = read(new_socket, payload_bytes + total_received, sizeof(payload) - total_received);
        if (received <= 0) {
            std::cerr << "Failed to read numbers from client!" << std::endl;
            close(new_socket);
            close(server_fd);
            return -1;
        }
        total_received += received;
    }

    int32_t first = ntohl(payload[0]);
    int32_t second = ntohl(payload[1]);
    sum = first + second;
    std::cout << "Server received numbers: " << first << " and " << second << std::endl;

    int32_t net_sum = htonl(sum);

    // Send the result back to the client
    ssize_t bytes_sent = send(new_socket, &net_sum, sizeof(net_sum), 0);
    if (bytes_sent != sizeof(net_sum)) {
        std::cerr << "Failed to send result to client!" << std::endl;
        close(new_socket);
        close(server_fd);
        return -1;
    }
    std::cout << "Server sent sum: " << sum << std::endl;

    close(new_socket);
    close(server_fd);
    return 0;
}
