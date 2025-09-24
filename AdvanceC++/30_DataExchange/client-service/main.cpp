#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> // For gethostbyname
#include <unistd.h>
#include <cstring>
#include <cstdint>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    int32_t numbers[2] = {5, 7};
    int32_t payload[2];
    int32_t result = 0;

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error!" << std::endl;
        return -1;
    }

    // Get the server (data-service) details by name
    server = gethostbyname("data-service");
    if (server == NULL) {
        std::cerr << "Host not found!" << std::endl;
        return -1;
    }

    // Set server address details
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);

    // Connect to the server (data-service)
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection to data-service failed!" << std::endl;
        return -1;
    }

    // Prepare payload in network byte order
    payload[0] = htonl(numbers[0]);
    payload[1] = htonl(numbers[1]);

    // Send the two integers to the server
    ssize_t bytes_sent = send(sock, payload, sizeof(payload), 0);
    if (bytes_sent != sizeof(payload)) {
        std::cerr << "Failed to send all data to server!" << std::endl;
        close(sock);
        return -1;
    }
    std::cout << "Client sent numbers: " << numbers[0] << " and " << numbers[1] << std::endl;

    // Receive the sum from the server
    ssize_t total_received = 0;
    char* result_bytes = reinterpret_cast<char*>(&result);
    while (total_received < static_cast<ssize_t>(sizeof(result))) {
        ssize_t received = read(sock, result_bytes + total_received, sizeof(result) - total_received);
        if (received <= 0) {
            std::cerr << "Failed to receive result from server!" << std::endl;
            close(sock);
            return -1;
        }
        total_received += received;
    }

    std::cout << "Client received sum: " << ntohl(result) << std::endl;

    close(sock);
    return 0;
}
