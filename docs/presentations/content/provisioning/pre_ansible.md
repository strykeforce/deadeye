+++
weight = 20
+++

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
