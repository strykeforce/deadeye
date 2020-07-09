.. _deployment:

**********
Deployment
**********

We use `Ansible <https://docs.ansible.com>`_ to automate the deployment of the Deadeye system and its prerequisite development tools and libraries. Ansible is run from a local host computer that has the Deadeye repository present (called *control node* by Ansible).

Currently Ansible can be run from any machine with Python 2 (version 2.7) or Python 3 (versions 3.5 and higher) installed. This includes Red Hat, Debian, CentOS, macOS, any of the BSDs, and so on. Windows is not supported for the control node, although it can run under the Windows Subsystem for Linux (WSL). See the `Windows FAQ <https://docs.ansible.com/ansible/latest/user_guide/windows_faq.html#can-ansible-run-on-windows>`_ for more information.

Ansible is a tool written in Python so we use `Pipenv <https://pipenv.pypa.io/en/latest/>`_ to manage our working Python environment.

.. note:: You may deploy Deadeye to multiple systems such as another Nano, macOS or Linux -- just be sure to give each a different unit ID (A, B, C, etc.).


Prerequisites
=============

#. Make sure you've go Python and pip and both are available from your command line. Help can be found `here <https://pipenv.pypa.io/en/latest/install/#make-sure-you-ve-got-python-pip>`_.

#. Install Pipenv using these `instructions <https://pipenv.pypa.io/en/latest/install/#installing-pipenv>`_.

#. The `Deadeye repository <https://github.com/strykeforce/deadeye>`_ has been cloned to your computer.

Go to the ansible directory in the Deadeye repository and install Ansible by running:

.. code-block:: shell-session

    $ cd deadeye/ansible
    $ pipenv install

Provisioning
============

Provisioning installs required development tools and libraries to build and run Deadeye system components. We'll assumed Ansible in the Deadeye repository is configured the way we want and provision the desired unit (deadeye-c, in this example):

.. code-block:: shell-session

    $ cd deadeye/ansible
    $ pipenv run provision -l deadeye-c

Building
========

Deployment builds and install Deadeye system components. To build and deploy Deadeye itself, run:

.. code-block:: shell-session

    $ cd deadeye/ansible
    $ pipenv run deploy -l deadeye-c