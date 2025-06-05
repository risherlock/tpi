#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "serial.h"
#include "config.h"

#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>

#define PORT 8080
#define BUF_SIZE 1024

char serial_rx_buf[PI_RX_BUFFER_LEN] = {0};

// Helper: Get IP of interface
std::string get_local_ip() {
    struct ifaddrs *ifaddr, *ifa;
    char ip[INET_ADDRSTRLEN];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return "Unknown";
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET &&
            !(ifa->ifa_flags & IFF_LOOPBACK)) {
            
            struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &(sa->sin_addr), ip, INET_ADDRSTRLEN);
            freeifaddrs(ifaddr);
            return std::string(ip);
        }
    }

    freeifaddrs(ifaddr);
    return "Unknown";
}

int read_line_from_serial(struct sp_port* port, char* buffer, size_t buffer_size) {
  size_t total_len = 0;
  bool in_frame = false;

  while (total_len < buffer_size - 1) { // Leave space for null terminator
    char byte;
    int rx = serial_read(port, &byte, 1);  // Read 1 byte at a time

    if (rx > 0) {
      if (!in_frame) {
        // Wait for start marker
        if (byte == '$') {
          in_frame = true;
          buffer[0] = byte;
          total_len = 1;
        }
        continue;
      }

      buffer[total_len++] = byte;

      if (byte == '#') {
        buffer[total_len] = '\0'; // Null-terminate
        return total_len;     // Success
      }

      // Optionally: detect corrupted/too-long frame
    } else if (rx < 0) {
      std::cerr << "Serial read error!" << std::endl;
      return -1;
    } else {
      // No data, wait a bit
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
  }

  buffer[buffer_size - 1] = '\0'; // Safety
  return 0; // Frame too long or not terminated
}

int main()
{
  // -------------------- Serial Init --------------------
  struct sp_port *port = NULL;
  const char *port_name = PI_SERIAL_PORT;

  if (!serial_init(port_name, &port)) {
  std::cerr << "Failed to initialize serial port" << std::endl;
  return 1;
  }

  std::cerr << "Serial port initialized successfully!" << std::endl;

  // -------------------- UDP Init -----------------------
  int sockfd;
  char udp_rx_buf[BUF_SIZE];
  struct sockaddr_in servaddr{}, cliaddr{};

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
  perror("socket creation failed");
  return 1;
  }

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
  {
    perror("bind failed");
    close(sockfd);
    return 1;
  }

  std::cout << "UDP Server listening on port " << PORT << std::endl;
  std::string local_ip = get_local_ip();
  std::cout << "UDP Server listening on IP " << local_ip << ", port " << PORT << std::endl;

  socklen_t len = sizeof(cliaddr);
  bool client_connected = false;

  while (true)
  {
    int n = recvfrom(sockfd, udp_rx_buf, sizeof(udp_rx_buf) - 1, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &len);

  // Telecommands from GS
  if (n > 0)
  {
    udp_rx_buf[n] = '\0';
    std::string received(udp_rx_buf);

    if
    (received == "Hello from Qt")
    {
    std::cout << "Client connected: " << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << std::endl;
    client_connected = true;
    }
    else if (received == "Bye from Qt")
    {
    std::cout << "Client disconnected: " << inet_ntoa(cliaddr.sin_addr) << ":" << ntohs(cliaddr.sin_port) << std::endl;
    client_connected = false;
    }
    else // Forward telecommand to STM32
    {
      int tx_len = sp_nonblocking_write(port, udp_rx_buf, n);
      
      if (tx_len < 0)
      {
        std::cerr << "Write error: " << sp_last_error_message() << std::endl;
      }
      else
      {
        sp_drain(port);
      }
      std::cout << "Unknown message: " << received << std::endl;
    }
  }

  // Forward serial data to GS
  if (client_connected)
  {
    // int rx_len = serial_read(port, serial_rx_buf, sizeof(serial_rx_buf));
    int rx_len = read_line_from_serial(port, serial_rx_buf, sizeof(serial_rx_buf));
// if (rx_len > 0) {
//   std::cout << "Received line: " << serial_rx_buf;
// }


    if (rx_len > 0)
    {
    // std::cout << "Received " << rx_len << " bytes: " << serial_rx_buf << std::endl;

    int sent = sendto(sockfd, serial_rx_buf, rx_len, 0, (struct sockaddr *)&cliaddr, len);
    memset(serial_rx_buf, 0, sizeof(serial_rx_buf));

    if (sent < 0)
    {
      perror("sendto failed");
    }
    } 
    else if (rx_len < 0)
    {
    std::cerr << "Serial read error: " << sp_last_error_message() << std::endl;
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  close(sockfd);
  sp_close(port);

  return 0;
}
