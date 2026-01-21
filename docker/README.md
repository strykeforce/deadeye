# Docker Installation and Usage

This is the preferred method of deploying and operating the Deadeye system.
Docker is readily available on most, if not all, Linux distributions used on
vision coprocessors.

## Installation

We'll use the [Docker Compose](https://docs.docker.com/compose/) tool to deploy
and operate Deadeye. The example commands below work on an Orange Pi running
their Ubuntu jammy [server image](http://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/service-and-support/Orange-pi-5.html).

```sh
# install commonly required packages
sudo apt install apt-transport-https ca-certificates curl gnupg-agent software-properties-common

# add the docker repository signing key
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | \
  sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

# add the docker repository
echo \
"deb [arch=$(dpkg --print-architecture) \
signed-by=/etc/apt/keyrings/docker.gpg] \
https://download.docker.com/linux/ubuntu \
$(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

# update the package lists
sudo apt update

# install docker
sudo apt install docker-ce docker-ce-cli containerd.io docker-compose-plugin
```

### Docker permission

Add the account you are using (`orangepi` in our case) to the `docker` group.

```sh
sudo usermod -a -G docker orangepi
# login again to take effect
```

### Docker Compose

Edit the `docker-compose.yaml` file as appropriate for your system. Select the
[docker image tags](https://hub.docker.com/search?q=j3ff/deadeye) to use and
edit the lines containing:

```yaml
daemon:
  image: j3ff/deadeye-daemon:<TAG>
admin:
  image: j3ff/deadeye-admin:<TAG>
web:
  image: j3ff/deadeye-web:<TAG>
```

In most cases, using `latest` as the value for `<TAG>` above will work. Other
valid tags can be looked up on [Docker Hub](https://hub.docker.com/) by searching
for `j3ff/deadeye-daemon`, etc.

#### IMPORTANT: Environment Variables to Configure

Before running Deadeye, you must configure the following environment variables:

##### In `docker-compose.yaml`

- `DEADEYE_STREAM_HOST` - the IP address of the host that is running the Deadeye
server software, for example, `10.27.67.13`.

**Note:** All other environment variables in `docker-compose.yml` should be left
with their default values.

##### In `deadeye.service`

- `DOCKER_COMPOSE_FILE` - The path to the `docker-compose.yml` file.
- `CAMERA_PATH` - The path to the camera being used inside the `/dev` directory.

```systemd
Environment="DOCKER_COMPOSE_FILE=/home/orangepi/deadeye/docker-compose.yml"
Environment="CAMERA_PATH=/dev/v4l/by-id/usb-Microsoft_Microsoft®_LifeCam_HD-3000-video-index0"
```

### systemd

The Deadeye containers are run using systemd instead of normal container restart
policies to work around Docker issues with camera device names.

After editing the required environment variables in `deadeye.service` (see
above), copy this file to `/etc/systemd/system/deadeye.service` and reload
systemd services with:

```sh
systemctl daemon-reload
```

## Operation

Start the Deadeye containers with:

```sh
sudo systemctl start deadeye
```

Stop the Deadeye containers with:

```sh
sudo systemctl stop deadeye
```

View container logs:

```sh
# in directory containing docker-compose.yaml
docker compose logs -f

# or alternatively, where SERVICE is one of: admin, daemon, web
docker compose logs $SERVICE
```

Updating the containers to a new version:

```sh
sudo systemctl stop deadeye

# in directory containing docker-compose.yaml
# edit the docker image tag(s) in docker-compose.yaml
docker compose pull

# it's a good idea to recreate the docker network
docker compose down

sudo systemctl start deadeye
```

Clean up old docker images and containers.

```sh
docker system prune
```
