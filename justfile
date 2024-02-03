# just manual: https://github.com/casey/just#readme

default:
  @just --list

deploy: docker
  @just -f admin/justfile deploy
  @just -f daemon/justfile deploy
  @just -f web/justfile deploy
