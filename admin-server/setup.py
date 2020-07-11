from setuptools import setup, find_packages

setup(
    name="deadeye",
    version="20.2.0",  # keep in sync with ansible/roles/deadeye/vars/main.yaml, daemon/version.cmake
    long_description=__doc__,
    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,
)

