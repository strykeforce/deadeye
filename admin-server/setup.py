from setuptools import setup, find_packages

setup(
    name="deadeye",
    version="20.3.0",  # updated by scripts/bump.py
    long_description=__doc__,
    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,
)

