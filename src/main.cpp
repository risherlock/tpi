#include <chrono>
#include <iostream>

#include "serial.h"
#include "config.h"

int main(void)
{
  struct sp_port *port = NULL;
  const char *port_name = PI_SERIAL_PORT;

  if (!serial_init(port_name, &port))
  {
    std::cerr << "Failed to initialize serial port" << std::endl;
    return 1;
  }

  std::cerr << "Serial port initialized successfully!" << std::endl;

  char rx_buf[PI_RX_BUFFER_LEN] = {0};
  int rx_len = 0;

  while (1)
  {
    rx_len = serial_read(port, rx_buf, sizeof(rx_buf));
  
    if (rx_len > 0)
    {
      auto now = std::chrono::system_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

      std::cout << ms << ": ";
      std::cout.write(rx_buf, rx_len);
    }
    else if (rx_len < 0)
    {
      std::cerr << "Error: Could not read from the serial port" << std::endl;
      break;
    }
  }

  std::cout << "Program exiting" << std::endl;
  return 0;
}
