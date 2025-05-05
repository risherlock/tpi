#include <iostream>
#include <sys/file.h>

#include "serial.h"

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

  int fd;
  if (sp_get_port_handle(*port, &fd) != SP_OK) {
    std::cerr << "Error: Failed to get port handle" << std::endl;
    sp_close(*port);
    sp_free_port(*port);
    return false;
  }

  if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
    std::cerr << "Error: Port is locked by another process" << std::endl;
    sp_close(*port);
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

int serial_read(sp_port *port, char *buf, const int buf_len)
{
  return sp_nonblocking_read(port, buf, buf_len);
}
