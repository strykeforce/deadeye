.. _sect_install:

*******************
|NANO| Installation
*******************

This section describes how to prepare a |JETSON|_ for Deadeye deployment. We deploy the base Linux OS, and prepare the system for automated provisioning in the next section (see :ref:`sect_deployment`).

.. note:: This step is only done once for each |NANO| SD card you prepare.

.. contents:: Topics

|JETSON|
========

When you boot the first time from a freshly-imaged SD card, the |NANO| will take you through some initial set-up. Normally this is done with an attached monitor, keyboard and mouse, but for convenience we will perform a "headless" set-up over a USB serial connection.

Items Required
--------------

Have the following items available before starting installation.

**microSD Card**
    The |NANO| uses a microSD card as a boot device and for main storage. It’s important to have a card that’s fast and large enough for your projects; the minimum recommended is a 16GB UHS-1 card.

**5V 4A Power Supply**
    Normally the |NANO| is powered via the micro-USB connector, however we will be using it for serial communication during set-up. You'll need to power the |NANO| with a DC barrel jack power supply (with 5.5mm OD / 2.1mm ID / 9.5mm length, center pin positive) that can supply up to 5V 4A. See `Adafruit’s 5V 4A switching power supply <https://www.adafruit.com/product/1466>`_.

**Circuit Board Jumper Cap**
    To enable use of the DC barrel jack power supply, you will need to fit a jumper to J48 on the |NANO| baseboard.

**USB Cable**
    You will use a micro-USB to USB A cable to connect to the |NANO|. Make sure that the cable is data capable. Some USB cables (such as those that come with phones) only transmit power.

**Ethernet Cable**
    For connecting to your network.

First Boot
----------

.. note:: Start with the |NANO| unpowered; we will connect to power supply in a step below.


#. Download the `|NANO| Developer Kit SD Card image <https://developer.nvidia.com/embedded/jetpack>`_ and install it to your microSD card using `balenaEtcher <https://www.balena.io/etcher/>`_ or equivalent.

#. Install the microSD card into the slot on the underside of the |NANO| module.

#. Connect the USB cable between your computer and the Nano micro-USB connector.

#. Connect the ethernet cable to your network.

#. Attach the circuit board jumper cap across ``J48``.

#. Attach the 5v power supply to the ``J25 power jack`` to boot the |NANO|.

#. When boot is complete, connect to the |NANO| via USB serial.  You will need a serial terminal application such as **puTTY** for Windows or **screen** for Linux and macOS. More information about connecting with a serial terminal can be found at `JetsonHacks <https://www.jetsonhacks.com/2019/08/21/jetson-nano-headless-setup/>`_.

Initial Set-up
--------------

When you boot the first time, the |NANO| Developer Kit will take you through some initial setup, including:

#. Review and accept NVIDIA |JETSON| software EULA

#. Select system language, location, and time zone

#. System clock set to UTC: **Yes**

#. User full name: **Deadeye**

#. Username: **deadeye**

#. Password: *use shop WiFi password*

#. APP partition size: accept default value to use entire SD card

#. Network configuration: select **eth0** as the primary network interface. If you are connected to a network via ethernet, you should be assigned an IP address using DHCP. We will configure a static IP address below.

#. Select a host name corresponding to the unit ID, for example, unit A is **deadeye-a**.

#. Log back into the |NANO| as user **deadeye** and reboot using ``sudo reboot``.

2767 Configuration
==================

For team 2767, computer name and IP address are according to the following table:

    ==== =========  ===========
    Unit Hostname   IP Address
    ==== =========  ===========
    A    deadeye-a  10.27.67.10
    B    deadeye-b  10.27.67.11
    C    deadeye-c  10.27.67.12
    D    deadeye-d  10.27.67.13
    ==== =========  ===========

The gateway and DNS server are **10.27.67.1** for all units.

.. important:: Make sure you have rebooted the |NANO| after performing initial set-up since its network interface will change after first boot.

To finish configuration, log in as user **deadeye** and run the following command to install the ``curl`` utility:

.. code-block:: console

    $ sudo apt install -y curl

When the ``curl`` utility is successfully installed, run our custom bootstrap script:

.. code-block:: console

    $ curl https://www.strykeforce.org/deadeye/bootstrap.sh | sudo bash

Reboot the |NANO| using ``sudo reboot`` and confirm you can log in remotely from your computer via SSH: ``ssh deadeye@10.27.67.12`` (deadeye-c).

Miscellaneous
=============

Update installed packages, this may take a while depending on the number of out-of-date packages:

.. code-block:: console

    $ sudo apt upgrade

