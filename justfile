# just manual: https://github.com/casey/just#readme

default:
  @just --list

deploy:
  just admin/deploy
  just daemon/deploy
  just web/deploy
