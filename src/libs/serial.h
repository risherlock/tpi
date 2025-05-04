#ifndef _TPI_SERIAL_H_
#define _TPI_SERIAL_H_

#include <libserialport.h>

bool serial_init(const char *port_name, struct sp_port **port);
int serial_read(sp_port *port, char *buf, const int buf_len);

#endif // serial.h
