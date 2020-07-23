#!/usr/bin/env python3
"""Bump Deadeye to specified version."""

import argparse
from pathlib import Path


def do_cmake(version):
    """ Bump cmake version file."""
    cmake = Path(__file__).parent / ".." / "daemon" / "version.cmake"

    with open(cmake, "r") as file:
        lines = file.readlines()

    lines[0] = f"set(Deadeye_VER {version}) # updated by scripts/bump.py\n"

    with open(cmake, "w") as file:
        file.writelines(lines)


def do_setup(version):
    """ Bump setup.py version."""
    setup = Path(__file__).parent / ".." / "admin-server" / "setup.py"

    with open(setup, "r") as file:
        lines = file.readlines()

    assert lines[4].startswith("    version=")
    lines[4] = f'    version="{version}",  # updated by scripts/bump.py\n'

    with open(setup, "w") as file:
        file.writelines(lines)


def do_ansible(version):
    """ Bump ansible vars version."""
    vars = (
        Path(__file__).parent
        / ".."
        / "ansible"
        / "roles"
        / "deadeye"
        / "vars"
        / "main.yaml"
    )

    with open(vars, "r") as file:
        lines = file.readlines()

    assert lines[1].startswith("deadeye_version:")
    lines[1] = f"deadeye_version: {version} # updated by scripts/bump.py\n"

    with open(vars, "w") as file:
        file.writelines(lines)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Bump Deadeye to specified version.")
    parser.add_argument(
        "version", metavar="VERSION", type=str, help="next version",
    )
    args = parser.parse_args()

    do_cmake(args.version)
    do_setup(args.version)
    do_ansible(args.version)
