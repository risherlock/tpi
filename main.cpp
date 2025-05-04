#include <chrono>
#include <iostream>
#include <libserialport.h>

#include "config.h"

bool serial_init(const char *port_name, struct sp_port **port)
{
  if (sp_get_port_by_name(port_name, port) != SP_OK)
  {
    std::cerr << "Error: Unable to find port " << port_name << std::endl;
    return false;
  }

  if (sp_open(*port, SP_MODE_READ) != SP_OK)
  {
    std::cerr << "Error: Unable to open port for reading" << std::endl;
    sp_free_port(*port);
    return false;
  }

  if (sp_set_bits(*port, 8)                          != SP_OK ||
      sp_set_stopbits(*port, 1)                      != SP_OK ||
      sp_set_baudrate(*port, 115200)                 != SP_OK ||
      sp_set_parity(*port, SP_PARITY_NONE)           != SP_OK ||
      sp_set_flowcontrol(*port, SP_FLOWCONTROL_NONE) != SP_OK)
  {
    std::cerr << "Error: Failed to set serial port parameters" << std::endl;
    sp_close(*port);
    sp_free_port(*port);
    return false;
  }

  return true;
}

int main(void)
{
  struct sp_port *port = NULL;
  const char *port_name = PI_SERIAL_PORT;

  if (!serial_init(port_name, &port))
  {
    std::cerr << "Failed to initialize serial port" << std::endl;
    return 1;
  }

  std::cout << "Serial port initialized successfully!" << std::endl;

  char rx_buf[PI_RX_BUFFER_LEN] = {0};
  int rx_len;

  while (1)
  {
    rx_len = sp_nonblocking_read(port, rx_buf, sizeof(rx_buf));

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

  sp_close(port);
  sp_free_port(port);

  std::cout << "Program exiting" << std::endl;
  return 0;
}
