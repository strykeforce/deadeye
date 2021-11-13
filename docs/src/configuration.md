# Configuration

Deadeye pipelines are configured at build time by configuring CMake when
building directly during development or by editing `inventory.yaml` when
deploying with Ansible.

## Deadeye Pipelines

Deadeye has several general-purpose pipelines available that cover a
majority of use cases.

- `UprightRectPipeline`
- `MinAreaRectPipeline`
- `TargetListPipeline`

Deadeye servers have a configuration file at `/etc/opt/deadeye/deadeye.conf`

Deadeye vision server pipelines are configured by pointing your browser
to `http://<address of Deadeye unit>/`.
