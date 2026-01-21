# Deadeye Web Console

This web-based adminstration dashboard is used to control, configure and monitor
the camera pipeline daemon.

It is typically served from the Deadeye camera processor and communicates with
the web admin dashboard server over websockets and streams camera preview video
directly from the camera pipeline daemon over TCP as MJPEG.

## Developing

You can run this web application during development by installing dependencies:

```sh
npm install
```

Then you can start a development server:

```sh
npm start
```
