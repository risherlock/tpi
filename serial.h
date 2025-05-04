#ifndef _TPI_SERIAL_H_
#define _TPI_SERIAL_H_

#include <libserialport.h>

bool serial_init(const char *port_name, struct sp_port **port);
bool serial_read(const sp_port *port, char *buf, int *len);

#endif // serial.h
