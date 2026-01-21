{ pkgs, perSystem }:
pkgs.spdlog.override {
  # TODO: check version when upgrading wpilib
  fmt = perSystem.self.fmt-8;
}
