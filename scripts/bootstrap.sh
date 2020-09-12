#!/bin/bash

CONN=$(nmcli -g GENERAL.CONNECTION device show eth0)
UUID=$(nmcli -g connection.uuid connection show "$CONN")
HOSTNAME=$(hostname)

case "$HOSTNAME" in
	deadeye-a)
		IP=10.27.67.10
		;;
	deadeye-b)
		IP=10.27.67.11
		;;
	deadeye-c)
		IP=10.27.67.12
		;;
	deadeye-d)
		IP=10.27.67.13
		;;
	*)
		echo Unrecognized hostname: $HOSTNAME
		exit 1
		;;
esac

nmcli connection modify $UUID \
	connection.id ether-eth0 \
	ipv4.method manual \
	ipv4.addresses $IP/24 \
	ipv4.gateway 10.27.67.1 \
	ipv4.dns 10.27.67.1

nmcli connection down $UUID > /dev/null
nmcli connection up $UUID > /dev/null

echo Configured $HOSTNAME with static ip address $IP

echo '%sudo    ALL=(ALL:ALL) NOPASSWD:ALL' > /etc/sudoers.d/local
echo Allow sudo without password for Ansible

systemctl set-default multi-user.target
echo Graphical boot disabled
