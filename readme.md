# TamariwPi

Software for the RaspberryPi zero onboard the Tamariw satellite.

## Dependencies

1. git - `sudo apt install git`
2. libserialport - `sudo apt install libserialport-dev`

### Creating default serial port

1. Open the `/boot/firmware/config.txt` file, enable GPIO14 and GPIO15 for UART, and disable Bluetooth to make sure that serial pins are free for use by our serial port.

```
  sudo nano /boot/firmware/config.txt
  enable_uart=1
  dtoverlay=disable-bt
```

2. Disable login shell to be accessible via serial and enable serial port hardware.

```
  sudo raspi-config -> Interface Options -> Serial Port
```

3. Configure default serial port

```
  sudo nano /boot/firmware/cmdline.txt
  console=ttyAMA0,115200 console=tty1
```

4. Reboot `sudo reboot`
