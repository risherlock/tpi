# TamariwPi

Software for the RaspberryPi zero onboard the Tamariw satellite.

## Dependencies

1. libserialport - `sudo apt install libserialport-dev`

## Notes

### Creating default serial port

1. Open the `/boot/config.txt` file, enable GPIO14 and GPIO15 for UART, and disable Bluetooth to make sure that serial pins are free for use by our serial port.

```
  sudo nano /boot/config.txt
  enable_uart=1
  dtoverlay=disable-bt
```

2. Disable console login via serial

```
  sudo raspi-config -> Interface Options -> Serial -> No
```

3. Configure default serial port

```
  sudo nano /boot/cmdline.txt
  console=ttyAMA0,115200 console=tty1
```

4. Reboot `sudo reboot`
