+++
title = "Jetson Nano Provisioning"
outputs = ["Reveal"]
+++

# Jetson Nano Provisioning

Preparing a fresh install of Deadeye on a Jetson Nano.

---

## First Boot

1. Download Jetson Nano [image file](#) and install to SD card using [Etcher](https://www.balena.io/etcher/) or equivalent.
2. Install SD card, attach keyboard, mouse and monitor.
3. Boot and accept the license agreement.

---

Allow use of `sudo` without password:

```sh
> sudo visudo
> # then edit the following line to read
%sudo   ALL=(ALL:ALL) NOPASSWD:ALL
```

Disable the graphical boot:

```sh
> sudo systemctl set-default multi-user.target
```
