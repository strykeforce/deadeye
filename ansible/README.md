# Deadeye Ansible Provisioning

## Provision

Install build dependencies for Deadeye on Ubuntu.

```sh
$ ansible-playbook -i inventory.yaml --vault-password-file=.vaultpw provision.yaml -l deadeye-X
```

## Deploy

Builds and deploys latest version in GitHub main branch.

```sh
$ ansible-playbook -i inventory.yaml --vault-password-file=.vaultpw deploy.yaml -l deadeye-X
```
