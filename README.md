# Deadeye

**Deadeye** is a vision system for FIRST<sup>®</sup> Robotics Competition
robots designed to be run on a vision coprocessor, such as a Linux single-board
computer. It has been actively used with the [Jetson Nano Developer Kit][jndk]
and [Orange Pi 5 Plus][orangepi]. It consists of several parts:

[jndk]: https://developer.nvidia.com/EMBEDDED/jetson-nano-developer-kit/
[orangepi]: http://www.orangepi.org/index.html

- Vision Daemon - C++ vision capture and processing pipeline.
- Client Library - Java library for use with FRC robots and other clients
- Admin Web UI - React web interface to manage Deadeye vision system.
- Admin Server and Tools - Python web service to connect admin web UI to vision
  daemon backend.
- Deployment Tools - Ansible playbooks for provisioning and deploying the
  system to vision coprocessors. **This method of deployment is deprecated in favor of using Docker (see below).**

## Documentation

- [Installation and usage instructions](https://strykeforce.github.io/deadeye/) - these instructions are deprecated in favor of using Docker (see below).
- The Java client library [javadocs](https://strykeforce.github.io/deadeye/javadoc/)

## Docker Installation and Usage

This is the preferred method of deploying and operating the Deadeye system.
Docker is readily available on most, if not all, Linux distributions used on
vision coproccessors. See the [installation and usage instructions](./docker/)
in the docker subdirectory of this repo.

## Contributing

Deadeye is free and open source. You can find the source code on
[GitHub](https://github.com/strykeforce/deadeye) and issues and feature
requests can be posted on the [GitHub issue
tracker](https://github.com/strykeforce/deadeye/issues). Deadeye relies on our
users to fix bugs and add features: if you'd like to contribute, please
consider opening a [pull
request](https://github.com/strykeforce/deadeye/pulls).
