# Deadeye Web Console

This web-based adminstration dashboard is used to control, configure and monitor the camera pipeline daemon.

It is typically served from the Deadeye camera processor and communicates with the web admin dashboard server over websockets and streams camera preview video directly from the camera pipeline daemon over TCP as MJPEG.

## Developing

You can run this web application during development by installing dependencies:

```sh
npm install
```

Then you can start a development server:

```sh
npm start
```

## NixOS

A Nix flake provides a package and module for running this on Nix-enabled systems. To build:

```sh
nix build ..#web
ls result/ # view build
```

### Configuration

To rebuild the `node2nix` configuration:

```sh
node2nix -18 -l package-lock.json -o config/node-packages.nix -c config/default.nix -e config/node-env.nix
```

### Container Testing

On NixOS you can create a container with the `nixos-container` command for testing:

```sh
sudo nixos-container create deadeye --flake .#container
host IP is 10.233.1.1, container IP is 10.233.1.2
```

Then you can start the container with the command:

```sh
sudo nixos-container start deadeye
```

You can update the container's configuration with this command:

```sh
sudo nixos-container update deadeye --flake .#container
reloading container...
```

You can delete this container with `sudo nixos-container destroy deadeye` when you are done with it.
