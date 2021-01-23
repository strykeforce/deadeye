#!/usr/bin/env python3
"""Manage Deadeye version."""

import argparse
from pathlib import Path
import re


def displaymatch(match):
    if match is None:
        return None
    return "<Match: %r, groups=%r>" % (match.group(), match.groups())


class Version:
    """Manage Deadeye version."""

    def __init__(self):
        self.deadeye_dir = Path(__file__).parent / ".."
        self.major_version = 0
        self.minor_version = 0
        self.patch_version = 0

        version_parser = re.compile(r"set\(Deadeye_VER\s(\d+)\.(\d+)\.(\d+)")
        cmake = self.deadeye_dir / "daemon" / "version.cmake"
        with open(cmake, "r") as file:
            for line in file.readlines():
                match = version_parser.match(line)
                # print(displaymatch(match))
                if match:
                    self.major_vers = int(match.group(1))
                    self.minor_vers = int(match.group(2))
                    self.patch_vers = int(match.group(3))

    def version(self):
        return f"{self.major_vers}.{self.minor_vers}.{self.patch_vers}"

    def bump_major(self):
        self.major_vers = self.major_vers + 1
        self.minor_vers = 0
        self.patch_vers = 0

    def bump_minor(self):
        self.minor_vers = self.minor_vers + 1
        self.patch_vers = 0

    def bump_patch(self):
        self.patch_vers = self.patch_vers + 1

    def update_daemon(self):
        """ Bump cmake version file."""
        cmake = self.deadeye_dir / "daemon" / "version.cmake"

        with open(cmake, "r") as file:
            lines = file.readlines()

        lines[0] = f"set(Deadeye_VER {self.version()}) # updated by scripts/bump.py\n"

        with open(cmake, "w") as file:
            file.writelines(lines)

    def update_admin(self):
        """ Bump setup.py version."""
        setup = self.deadeye_dir / "admin" / "setup.py"

        with open(setup, "r") as file:
            lines = file.readlines()

        assert lines[5].startswith("    version=")
        lines[5] = f'    version="{self.version()}",  # updated by scripts/bump.py\n'

        with open(setup, "w") as file:
            file.writelines(lines)

    def update_ansible(self):
        """ Bump ansible vars version."""
        ans = self.deadeye_dir / "ansible" / "roles" / "deadeye" / "vars" / "main.yaml"

        with open(ans, "r") as file:
            lines = file.readlines()

        assert lines[1].startswith("deadeye_version:")
        lines[1] = f"deadeye_version: {self.version()} # updated by scripts/bump.py\n"

        with open(ans, "w") as file:
            file.writelines(lines)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Bump Deadeye to specified version.")
    parser.add_argument("--major", action="store_true")
    parser.add_argument("--minor", action="store_true")
    parser.add_argument("--patch", action="store_true")
    args = parser.parse_args()

    version = Version()
    if args.major:
        version.bump_major()
    if args.minor:
        version.bump_minor()
    if args.patch:
        version.bump_patch()

    version.update_daemon()
    version.update_admin()
    version.update_ansible()

    print(f"Updated version to {version.version()}")
