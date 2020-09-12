# Deadeye Vision System

[![Documentation Status](https://readthedocs.org/projects/deadeye/badge/?version=latest)](https://deadeye.readthedocs.io/en/latest/?badge=latest)

This is a vision system for FRC robots, designed to be run on a vision coprocessor. It consists of three parts:

- [Vision Daemon](./daemon/) - C++ vision capture and processing pipeline.
- [Admin Web UI](./admin-web) - React web interface to manage Deadeye vision system.
- [Admin Server and Tools](./admin/) - Python web service to connect admin web UI to vision daemon back-end.
