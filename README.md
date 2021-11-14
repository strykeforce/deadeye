# Deadeye

**Deadeye** is a vision system for FIRST<sup>®</sup> Robotics Competition
robots designed to be run on a vision coprocessor, such as the [Jetson Nano
Developer Kit][jndk]. It consists of several parts:

[jndk]: https://developer.nvidia.com/EMBEDDED/jetson-nano-developer-kit/

- Vision Daemon - C++ vision capture and processing pipeline.
- Client Library - Java library for use with FRC robots and other clients
- Admin Web UI - React web interface to manage Deadeye vision system.
- Admin Server and Tools - Python web service to connect admin web UI to vision
  daemon back-end.
- Provisioning Tools - Ansible playbooks for provisioning and deploying the
  system to vision coprocessors.

## Contributing

Deadeye is free and open source. You can find the source code on
[GitHub](https://github.com/strykeforce/deadeye) and issues and feature
requests can be posted on the [GitHub issue
tracker](https://github.com/strykeforce/deadeye/issues). Deadeye relies on our
users to fix bugs and add features: if you'd like to contribute, please
consider opening a [pull
request](https://github.com/strykeforce/deadeye/pulls).
