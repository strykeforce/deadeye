# Installation

> Note: This installation method is deprecated in favor of running Deadeye in
> Docker on vision coprocessors. See Docker installation and usage
> [instructions](https://github.com/strykeforce/deadeye/tree/main/docker).

This section describes how to prepare a Jetson Nano for Deadeye deployment. We
deploy the base Linux OS, and prepare the system for automated provisioning in
the next section.

> Note: This step is only done once for each SD card you prepare.

## Jetson Nano Developer Kit

When you boot the first time from a freshly-imaged SD card, the will take you
through some initial set-up. Normally this is done with an attached monitor,
keyboard and mouse, but for convenience we will perform a "headless" set-up
over a USB serial connection.

### Items Required

Have the following items available before starting installation.

**microSD Card:** The uses a microSD card as a boot device and for main
storage. It's important to have a card that's fast and large enough for your
projects; the minimum recommended is a 16GB UHS-1 card.

**5V 4A Power Supply:** Normally the is powered via the micro-USB connector,
however we will be using it for serial communication during set-up. You'll
need to power the with a DC barrel jack power supply (with 5.5mm OD / 2.1mm ID
/ 9.5mm length, center pin positive) that can supply up to 5V 4A. See
[Adafruit's 5V 4A switching power supply][adafruit-ps].

[adafruit-ps]: https://www.adafruit.com/product/1466

**Circuit Board Jumper Cap:** To enable use of the DC barrel jack power supply,
you will need to fit a jumper to J48 on the baseboard.

**USB Cable:** You will use a micro-USB to USB A cable to connect to the Nano.
Make sure that the cable is data capable. Some USB cables (such as those that
come with phones) only transmit power.

**Ethernet Cable:** For connecting to your network.

### First Boot

> Note: Start with the Nano un-powered; we will connect to power supply in a
> step below.

1.  Download the [Jetson NANO Developer Kit SD Card image][sd-card] and install
    it to your microSD card using [balenaEtcher][balena] or equivalent.
2.  Install the microSD card into the slot on the underside of the
    module.
3.  Connect the USB cable between your computer and the Nano micro-USB
    connector.
4.  Connect the ethernet cable to your network.
5.  Attach the circuit board jumper cap across `J48`.
6.  Attach the 5v power supply to the `J25` power jack to boot the Nano.
7.  When boot is complete, connect to the via USB serial. You will need a
    serial terminal application such as `puTTY` for Windows or `screen` for
    Linux and macOS. More information about connecting with a serial terminal
    can be found at [JetsonHacks][jetson-hacks].

[sd-card]: https://developer.nvidia.com/embedded/jetpack
[balena]: https://www.balena.io/etcher/
[jetson-hacks]: https://www.jetsonhacks.com/2019/08/21/jetson-nano-headless-setup/

### Initial Set-up

When you boot the first time, the Developer Kit will take you through some
initial setup, including:

1.  Review and accept NVIDIA software EULA
2.  Select system language, location, and time zone
3.  System clock set to UTC: **Yes**
4.  User full name: **Deadeye**
5.  Username: **deadeye**
6.  Password: _use shop WiFi password_
7.  APP partition size: accept default value to use entire SD card
8.  Network configuration: select **eth0** as the primary network
    interface. If you are connected to a network via ethernet, you
    should be assigned an IP address using DHCP. We will configure a
    static IP address below.
9.  Select a host name corresponding to the unit ID, for example, unit A
    is **deadeye-a**.
10. Log back into the as user **deadeye** and reboot using `sudo reboot`

## 2767 Configuration

For team 2767, computer name and IP address are according to the
following table:

| Unit | Hostname  | IP Address  |
| ---- | --------- | ----------- |
| A    | deadeye-a | 10.27.67.10 |
| B    | deadeye-b | 10.27.67.11 |
| C    | deadeye-c | 10.27.67.12 |
| D    | deadeye-d | 10.27.67.13 |

The gateway and DNS server are **10.27.67.1** for all units.

> **Important:** Make sure you have rebooted the after performing initial
> set-up since its network interface will change after first boot.

To finish configuration, log in as user _deadeye_ and run the following
command to install the `curl` utility:

```console
$ sudo apt install -y curl
```

When the `curl` utility is successfully installed, run our custom bootstrap
script:

```console
$ curl https://www.strykeforce.org/deadeye/bootstrap.sh | sudo bash
```

Reboot the using `sudo reboot` and confirm you can log in remotely from your
computer via SSH: `ssh deadeye@10.27.67.12` (deadeye-c).

## Miscellaneous

Update installed packages, this may take a while depending on the number of
out-of-date packages:

```console
$ sudo apt upgrade
```
