#!/usr/bin/env python3
"""Manage Deadeye version."""

import argparse
import re
from pathlib import Path


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
        """Bump cmake version file."""
        cmake = self.deadeye_dir / "daemon" / "version.cmake"

        with open(cmake, "r") as file:
            lines = file.readlines()

        lines[0] = f"set(Deadeye_VER {self.version()}) # updated by scripts/bump.py\n"

        with open(cmake, "w") as file:
            file.writelines(lines)

    def update_admin(self):
        """Bump pyproject.toml version."""
        pyproject = self.deadeye_dir / "admin" / "pyproject.toml"

        with open(pyproject, "r") as file:
            lines = file.readlines()

        assert lines[5].startswith("version = ")
        lines[5] = f'version = "{self.version()}"  # updated by scripts/bump.py\n'

        with open(pyproject, "w") as file:
            file.writelines(lines)

        mod_init = self.deadeye_dir / "admin" / "deadeye" / "__init__.py"

        with open(mod_init, "r") as file:
            lines = file.readlines()

        assert lines[0].startswith("__version__ = ")
        lines[0] = f'__version__ = "{self.version()}"  # updated by scripts/bump.py\n'

        with open(mod_init, "w") as file:
            file.writelines(lines)

    def update_ansible(self):
        """Bump ansible vars version."""
        ans = self.deadeye_dir / "ansible" / "roles" / "deadeye" / "vars" / "main.yaml"

        with open(ans, "r") as file:
            lines = file.readlines()

        assert lines[1].startswith("deadeye_version:")
        lines[1] = f"deadeye_version: {self.version()} # updated by scripts/bump.py\n"

        with open(ans, "w") as file:
            file.writelines(lines)

    def update_client(self):
        """Bump Java client library version."""
        gradle = self.deadeye_dir / "client" / "build.gradle.kts"

        with open(gradle, "r") as file:
            lines = file.readlines()

        assert lines[7].startswith("version = ")
        lines[7] = f'version = "{self.version()}" // updated by scripts/bump.py\n'

        with open(gradle, "w") as file:
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
    version.update_client()

    print(f"Updated version to {version.version()}")
