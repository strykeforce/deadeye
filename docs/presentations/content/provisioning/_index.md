+++
title = "Jetson Nano Provisioning"
outputs = ["Reveal"]
[reveal_hugo]
custom_css = "css/custom.css"
+++

# Jetson Nano Initial Set-Up

Preparing a fresh install of Deadeye on a Jetson Nano.

---

## Nano First Boot

1.  Download Jetson Nano [SD card image](https://developer.nvidia.com/embedded/jetpack) and install to SD card using [Etcher](https://www.balena.io/etcher/).

2.  With the Nano unpowered, install the SD card.
3.  Connect a USB cable between a computer and the Nano micro-USB connector.

---

## Nano Power Connection

We will power the Nano with its 5v barrel connectior in order to connect via USB serial for a "headless" install.

1.  Attach a jumper across J48.
2.  Attach the 5v power supply to the J25 power jack.

{{< figure src="/nano/power.jpg" width=600 >}}
