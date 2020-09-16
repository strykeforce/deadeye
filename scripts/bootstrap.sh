#!/bin/bash


HOSTNAME=$(hostname)
DEADEYE=/opt/deadeye

function set_jetson_ip() {
	ip=$1
	conn=$(nmcli -g GENERAL.CONNECTION device show eth0)
	uuid=$(nmcli -g connection.uuid connection show "$conn")

	nmcli connection modify $UUID \
	connection.id ether-eth0 \
	ipv4.method manual \
	ipv4.addresses $ip/24 \
	ipv4.gateway 10.27.67.1 \
	ipv4.dns 10.27.67.1

	nmcli connection down $UUID > /dev/null
	nmcli connection up $UUID > /dev/null

	echo Configured $HOSTNAME with static ip address $IP
}

function config_sudo() {
	echo '%sudo    ALL=(ALL:ALL) NOPASSWD:ALL' > /etc/sudoers.d/local
	echo Allow sudo without password for Ansible
}

function disable_graphical_boot() {
	systemctl set-default multi-user.target
	echo Graphical boot disabled
}

function install_deadeye_cli() {
	apt-get install --yes --quiet git virtualenv
	[[ ! -e $DEADEYE/bin/python ]] &&  virtualenv -p python3 -q $DEADEYE
	[[ ! -d $DEADEYE/src ]] && git clone https://github.com/strykeforce/deadeye.git $DEADEYE/src
	$DEADEYE/bin/pip install --quiet --requirement=$DEADEYE/src/admin/requirements.txt
	cd $DEADEYE/src/admin
	$DEADEYE/bin/pip install .
	[[ ! -e /usr/local/bin/deadeye ]] && ln -s $DEADEYE/bin/deadeye /usr/local/bin/
	echo
	echo "Finish installation: deadeye update --provision"
}

if [ "$EUID" -ne 0 ]
  then echo "Please run as root: sudo $0"
  exit
fi

case "$HOSTNAME" in
	deadeye-a)
		set_jetson_ip 10.27.67.10
		;;
	deadeye-b)
		set_jetson_ip 10.27.67.11
		;;
	deadeye-c)
		set_jetson_ip 10.27.67.12
		;;
	deadeye-d)
		set_jetson_ip 10.27.67.13
		;;
	*)
		echo "Not a Jetson hostname: $HOSTNAME"
		echo "Will not set static IP address"
		;;
esac

config_sudo
disable_graphical_boot
install_deadeye_cli


