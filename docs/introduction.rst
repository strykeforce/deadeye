
.. _sect_intro:

************
Introduction
************

The Deadeye system has several components that run on the Jetson Nano, client roboRIO and web browser. They communicate over the network and require a running `NetworkTables <https://docs.wpilib.org/en/stable/docs/software/networktables/>`_ server.

**deadeye-daemon**
    The main vision processing process running on the Jetson Nano that manages cameras and associated target processing pipelines.
    
    It communicates via NetworkTables for configuration and control and via UDP to a **deadeye-java** client that uses streaming targeting data. It also provides an on-demand camera video stream directly to **deadeye-web** over TCP.

**deadeye-java**
    Client Java libary used by FRC roboRIO robot code to control and communicate with **deadeye-daemon**.
    
    It communicates via NetworkTables for configuration and control and to **deadeye-daemon** directly via UDP for streaming target data.

**deadeye-admin**
    A Python web service running on the Jetson Nano that is the backend for the web-based administration dashboard, **deadeye-web**, that configures and controls **deadeye-daemon**.
    
    It communicates with **deadeye-daemon** via NetworkTables and with **deadeye-web** over websockets.

**deadeye-web**
    The web-based adminstration dashboard run on a developer's computer used to control, configure and monitor **deadeye-daemon**.
    
    It communicates with **deadeye-admin** over websockets and streams camera video directly from **deadeye-deadeye** over TCP.

**deadeye-shutdown**
    A background service running on the Jetson Nano that watches for a shutdown button press and performs a clean shutdown.