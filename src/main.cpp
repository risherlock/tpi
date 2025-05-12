#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

#define PORT 8080

std::string generate_fake_sensor_data() {
    int temp = rand() % 10 + 20; // e.g., random 20–29 °C
    int humidity = rand() % 30 + 40; // e.g., random 40–69 %
    return "Temp: " + std::to_string(temp) + "°C, Humidity: " + std::to_string(humidity) + "%";
}

int main()
{
    srand(time(nullptr)); // seed random generator

    int sockfd;
    char buffer[1024];
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return 1;
    }

    std::cout << "UDP Server listening on port " << PORT << std::endl;

    socklen_t len = sizeof(cliaddr);

    while (true)
    {
        std::cout << "Waiting for client to send 'start'...\n";
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }

        buffer[n] = '\0';
        std::string received(buffer);
        std::cout << "Received: " << received << std::endl;

        if (received == "Hello from Qt") {
            std::cout << "Starting to send sensor data to client...\n";
            while (true) {
                std::string message = generate_fake_sensor_data();
                sendto(sockfd, message.c_str(), message.length(), 0,
                       (struct sockaddr *)&cliaddr, len);
                std::cout << "Sent: " << message << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // You can break this loop based on some condition or "stop" message
            }
        }
    }

    close(sockfd);
    return 0;
}
