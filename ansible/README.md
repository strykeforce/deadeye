# Deadeye Ansible Provisioning

**NOTE:** Using Ansible for provisioning and deployment is deprecated in favor
of using Docker on vision coprocessors.

## Check Connectivity

This will check IP address resolution and password-less sudo on target.

```sh
poetry run ansible -m ping -i inventory.yaml deadeye-X
```

## Provision

Install build dependencies for Deadeye on Ubuntu.

```sh
poetry run ansible-playbook -i inventory.yaml provision.yaml -l deadeye-X
```

## Deploy

Builds and deploys latest version in GitHub main branch.

```sh
poetry run ansible-playbook -i inventory.yaml deploy.yaml -l deadeye-X
```
