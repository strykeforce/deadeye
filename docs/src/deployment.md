# Deadeye Server Installation {#sect_deployment}

We use [Ansible](https://docs.ansible.com) to automate the deployment of
the Deadeye system and its prerequisite development tools and libraries.
Alternatively, you can install each of the Deadeye Server components
manually.

::: note
::: title
Note
:::

While the Deadeye server software can be installed on any Linux system,
these Ansible playbooks have only been tested on Ubuntu 18.04 and up.
:::

## Prerequisites

Ensure the following prerequisites are satisfied before installing the
Deadeye server software. Here, *deployment host* is the computer you are
running Ansible from (i.e. your development laptop).

1.  Your has a connection to the internet.
2.  The [strykeforce/deadeye
    repository](https://github.com/strykeforce/deadeye) has been cloned
    to the deployment host.
3.  Make sure you\'ve got **Python** and `pip`{.interpreted-text
    role="command"}, and both are available from the command line on the
    deployment host. Help can be found
    [here](https://pipenv.pypa.io/en/latest/install/#make-sure-you-ve-got-python-pip).
4.  The following build tools are installed on the deployment host:
    -   [Node.js](https://nodejs.org/en/download/)
    -   [Poetry](https://python-poetry.org)
    -   [Ansible](https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html)

## Server Inventory

The list of configured Deadeye servers is contained in the
`deadeye/ansible/inventory.yaml`{.interpreted-text role="file"} file.
This should be edited as needed when configuring new servers.

## Server Provisioning

Provisioning installs required development tools and libraries to build
and run Deadeye system components on a .

This step is typically only done once for each SD card you prepare. You
may decide to re-run provisioning if you change development tools or
library versions in Ansible.

We\'ll assumed Ansible in the `strykeforce/deadeye` repository is
configured the way we want and provision the desired unit (deadeye-c, in
this example):

``` console
$ cd deadeye/ansible
$ ansible-playbook -i inventory.yaml provision.yaml -l deadeye-c
```

## Deadeye Software Deployment

This step builds and install or upgrades Deadeye system components (see
`sect_intro`{.interpreted-text role="ref"}) on the and is run each time
you wish to update a with the most current version in the
`strykeforce/deadeye` repository.

To build and deploy Deadeye itself, first edit
`inventory.yaml`{.interpreted-text role="file"} in the project
`ansible`{.interpreted-text role="file"} subdirectory to configure the
build; most importantly, the pipeline assigned to each camera.

In the example below, **deadeye-a** is configured with
`deadeye::MinAreaRectPipeline` for camera A0 and
`deadeye::UprightRectPipeline` for camera A1.

``` yaml
deadeye:
hosts:
    deadeye-a:
    ansible_host: deadeye-a
    deadeye_unit: A
    camera_0_pipeline: deadeye::MinAreaRectPipeline
    camera_1_pipeline: deadeye::UprightRectPipeline
```

To actually start the build and installation, run:

``` console
$ cd deadeye/ansible
$ ansible-playbook -i inventory.yaml deploy.yaml -l deadeye-c
```
