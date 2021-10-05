.. _sect_deployment:

*********************
Server Software
*********************

Although typically used on a |JETSON|_, the Deadeye server software (vision
process daemon and admin dashboard web server) can be installed on any Linux
system with the appropriate C++ compiler and development libararies installed.


We use `Ansible <https://docs.ansible.com>`_ to automate the deployment of the
Deadeye system and its prerequisite development tools and libraries.
Alternatively, you can install each of the Deadeye Server components manually.

Ansible is run from a local host computer that has the ``strykeforce/deadeye``
repository present (called *control node* by Ansible).

Currently Ansible can be run from any machine with Python 2 (version 2.7) or
Python 3 (versions 3.5 and higher) installed. This includes Red Hat, Debian,
CentOS, macOS, any of the BSDs, and so on. Windows is not supported for the
control node, although it can run under the Windows Subsystem for Linux (WSL).
See the `Windows FAQ
<https://docs.ansible.com/ansible/latest/user_guide/windows_faq.html#can-ansible-run-on-windows>`_
for more information. 

.. note:: You may deploy Deadeye to multiple systems such as another Nano,
   macOS or Linux -- just be sure to give each a different unit ID (A, B, C,
   etc.).


Prerequisites
-------------

Ensure the following prerequisites are satisfied before installing the Deadeye
server software.

#. Your |NANO| has a connection to the internet.

#. The `strykeforce/deadeye repository
   <https://github.com/strykeforce/deadeye>`_ has been cloned to your computer.

#. Make sure you've got **Python** and :command:`pip`, and both are available
   from the command line. Help can be found `here
   <https://pipenv.pypa.io/en/latest/install/#make-sure-you-ve-got-python-pip>`_.
   
#. Build tools are installed on your development machine (not on |NANO|):

   * `Node.js <https://nodejs.org/en/download/>`_
   * `Poetry <https://python-poetry.org>`_
   * `Ansible <https://docs.ansible.com/ansible/latest/installation_guide/intro_installation.html>`_

Server Inventory
----------------

The list of configured Deadeye servers is contained in the ``inventory.yaml``
file. This should be edited as needed when configuring new servers.

Server Provisioning
-------------------

Provisioning installs required development tools and libraries to build and run
Deadeye system components on a |NANO|.

This step is typically only done once for each |NANO| SD card you prepare. You
may decide to re-run provisioning if you change development tools or library
versions in Ansible.

We'll assumed Ansible in the ``strykeforce/deadeye`` repository is configured
the way we want and provision the desired unit (deadeye-c, in this example):

.. code-block:: console

    $ cd deadeye/ansible
    $ ansible-playbook -i inventory.yaml provision.yaml -l deadeye-c

Deadeye Software Deployment
---------------------------

This step builds and install or upgrades Deadeye system components (see
:ref:`sect_intro`) on the |NANO| and is run each time you wish to update a
|NANO| with the most current version in the ``strykeforce/deadeye`` repository.


To build and deploy Deadeye itself, first edit :file:`inventory.yaml` in the
project :file:`ansible` subdirectory to configure the build; most importantly,
the pipeline assigned to each camera.

In the example below, **deadeye-a** is configured with
``deadeye::MinAreaRectPipeline`` for camera A0 and
``deadeye::UprightRectPipeline`` for camera A1.

.. code-block:: yaml

    deadeye:
    hosts:
        deadeye-a:
        ansible_host: deadeye-a
        deadeye_unit: A
        camera_0_pipeline: deadeye::MinAreaRectPipeline
        camera_1_pipeline: deadeye::UprightRectPipeline

To actually start the build and installation, run:

.. code-block:: console

    $ cd deadeye/ansible
    $ ansible-playbook -i inventory.yaml deploy.yaml -l deadeye-c
