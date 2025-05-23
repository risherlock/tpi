# TamariwPi

Software for the RaspberryPi zero onboard the Tamariw satellite.

## Dependencies

```
sudo apt update
```

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

### Permission to `/dev/ttyAMA0`

`sudo chmod +777 /dev/ttyAMA0` does the trick&mdash;but temporarily. In order to maintain the permissions on bootup, we have to make a udev rule for it.

1. Create a new udev rule

    ```
      sudo nano /etc/udev/rules.d/99-ttyama0.rules
    ```

2. Add the following line:

    ```
      KERNEL=="ttyAMA0", MODE="0666"
    ```

    This sets the permissions automatically to `rw-rw-rw-` every time `/dev/ttyAMA0` appears.

3. Reload and trigger udev rules

    ```
      sudo udevadm control --reload-rules
      sudo udevadm trigger
    ```

4. Fully disable the service. This prevents the service from being started manually or automatically.

    ```
      sudo systemctl mask serial-getty@ttyAMA0.service
    ```

4. Verify&mdash;after `sudo reboot`.

    ```
      ls -l /dev/ttyAMA0                            # Should show permissions 0666
      sudo lsof /dev/ttyAMA0                        # Should show no output
      systemctl status serial-getty@ttyAMA0.service # Should show "Active: inactive (dead)"
    ```

sudo systemctl disable ufw
sudo ufw disable

hostname -I # Hostname of the Pi in its local network 
Make sure to disable autoconnect to the router
