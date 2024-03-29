"""Deadeye Admin server and tools"""
from setuptools import find_packages, setup

setup(
    name="deadeye",
    version="21.0.0",  # updated by scripts/bump.py
    long_description=__doc__,
    packages=find_packages(),
    include_package_data=True,
    zip_safe=False,
    entry_points="""
        [console_scripts]
        deadeye-server=deadeye.scripts.server:main
        deadeye=deadeye.scripts.cli:main
    """,
)
