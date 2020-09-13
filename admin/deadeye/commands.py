import click
import os
import socket
import subprocess
import sys

DAEMON_SERVICE = "deadeye-daemon.service"
ADMIN_SERVICE = "deadeye-admin.service"
SHUTDOWN_SERVICE = "deadeye-shutdown.service"
MOUNT_SERVICE = "var-opt-deadeye.mount"

DEADEYE_DIR = "/opt/deadeye"
BIN_DIR = f"{DEADEYE_DIR}/bin"
ANSIBLE_DIR = f"{DEADEYE_DIR}/src/ansible"


@click.group()
@click.option("-v", "--verbose", is_flag=True)
@click.pass_context
def cli(ctx, verbose):
    """Update, control and monitor the Deadeye vision system."""
    ctx.ensure_object(dict)
    ctx.obj["VERBOSE"] = verbose
    pass


def run(ctx, args):
    if ctx.obj["VERBOSE"]:
        click.echo(f"{click.style('command', bold=True)}: {' '.join(args)}")
    sys.exit(subprocess.run(args).returncode)


@cli.command()
@click.option("-f", "--follow", is_flag=True, help="Follow the logs.")
@click.option("--all", is_flag=True, help="Show Deadeye admin web logs as well.")
@click.pass_context
def logs(ctx, follow, all):
    "View the Deadeye logs."
    args = ["/bin/journalctl", f"_SYSTEMD_UNIT={DAEMON_SERVICE}"]
    if follow:
        args.append("--follow")
    if all:
        args.append(f"_SYSTEMD_UNIT={ADMIN_SERVICE}")

    run(ctx, args)


@cli.command()
@click.pass_context
def status(ctx):
    "View the status of Deadeye services."
    run(
        ctx,
        [
            "/bin/systemctl",
            "status",
            DAEMON_SERVICE,
            ADMIN_SERVICE,
            SHUTDOWN_SERVICE,
            MOUNT_SERVICE,
        ],
    )


@cli.command()
@click.pass_context
def restart(ctx):
    "Restart Deadeye services."
    run(
        ctx,
        [
            "/bin/systemctl",
            "restart",
            DAEMON_SERVICE,
            ADMIN_SERVICE,
            SHUTDOWN_SERVICE,
            MOUNT_SERVICE,
        ],
    )


@cli.command()
@click.pass_context
def upgrade(ctx):
    "Upgrade Deadeye to latest version."
    os.chdir(ANSIBLE_DIR)
    if os.getenv("DEADEYE_MODE") == "development":
        os.chdir("/home/deadeye/deadeye/ansible")

    hostname = socket.gethostname()
    run(
        ctx,
        [
            f"{BIN_DIR}/ansible-playbook",
            "--connection=local",
            "--inventory=inventory.yaml",
            f"--limit={hostname}",
            "deploy.yaml",
        ],
    )
