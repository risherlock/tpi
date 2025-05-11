#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

#define PORT 8080

int main()
{
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
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[n] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        const char *reply = "Hello from Pi (UDP)!\n";
        sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
