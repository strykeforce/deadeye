.. _sect_deployment:

*********************
Software Installation
*********************

Deadeye Client Library
======================

Java Client library
-------------------

The Java client library can be included as a dependency of roboRIO robot programs.

The Third Coast :file:`vendordeps` file is at: http://maven.strykeforce.org/thirdcoast.json. To install, use :guilabel:`Install new libraries (online)` command in VS Code, download manually to your project :file:`vendordeps` directory, or use :command:`gradle vendordep` task supplied by GradleRIO in standard FRC projects.

.. code-block:: console

    $ ./gradlew vendordep --url=http://maven.strykeforce.org/thirdcoast.json



Deadeye Server Software
=======================

Although typically used on a |JETSON|_, the Deadeye server software (vision process daemon and admin dashboard web server) can be installed on any Linux system with the appropriate C++ compiler and development libararies installed.


We use `Ansible <https://docs.ansible.com>`_ to automate the deployment of the Deadeye system and its prerequisite development tools and libraries. Alternatively, you can install each of the Deadeye Server components manually.

Ansible is run from a local host computer that has the ``strykeforce/deadeye`` repository present (called *control node* by Ansible).

Currently Ansible can be run from any machine with Python 2 (version 2.7) or Python 3 (versions 3.5 and higher) installed. This includes Red Hat, Debian, CentOS, macOS, any of the BSDs, and so on. Windows is not supported for the control node, although it can run under the Windows Subsystem for Linux (WSL). See the `Windows FAQ <https://docs.ansible.com/ansible/latest/user_guide/windows_faq.html#can-ansible-run-on-windows>`_ for more information. 

Ansible is a tool written in Python so we use `Pipenv <https://pipenv.pypa.io/en/latest/>`_ to manage our working Python environment.

.. note:: You may deploy Deadeye to multiple systems such as another Nano, macOS or Linux -- just be sure to give each a different unit ID (A, B, C, etc.).


Server Prerequisites
--------------------

#. Your |NANO| has a connection to the internet.

#. Make sure you've got **Python** and :command:`pip`, and both are available from your command line. Help can be found `here <https://pipenv.pypa.io/en/latest/install/#make-sure-you-ve-got-python-pip>`_.

#. Install **Pipenv** using these `instructions <https://pipenv.pypa.io/en/latest/install/#installing-pipenv>`_.

#. The `strykeforce/deadeye repository <https://github.com/strykeforce/deadeye>`_ has been cloned to your computer.

Go to the :file:`ansible` directory in the ``strykeforce/deadeye`` repository and install Ansible by running:

.. code-block:: console

    $ cd deadeye/ansible
    $ pipenv install

Server Provisioning
-------------------

Provisioning installs required development tools and libraries to build and run Deadeye system components.

This step is typically only done once for each |NANO| SD card you prepare. You may decide to re-run provisioning if you change development tools or library versions in Ansible.

We'll assumed Ansible in the ``strykeforce/deadeye`` repository is configured the way we want and provision the desired unit (deadeye-c, in this example):

.. code-block:: console

    $ cd deadeye/ansible
    $ pipenv run provision -l deadeye-c

Installing Server Software
--------------------------

This step builds and install or upgrades Deadeye system components (see :ref:`sect_intro`) on the |NANO| and is run each time you wish to update a |NANO| with the most current version in the ``strykeforce/deadeye`` repository.


To build and deploy Deadeye itself, first edit :file:`inventory.yaml` in the project :file:`ansible` subdirectory to configure the build; most importantly, the pipeline assigned to each camera.

In the example below, **deadeye-a** is configured with ``deadeye::MinAreaRectPipeline`` for camera A0 and ``deadeye::UprightRectPipeline`` for camera A1.

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
    $ pipenv run deploy -l deadeye-a