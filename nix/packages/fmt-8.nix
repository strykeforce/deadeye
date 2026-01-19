{ pkgs }:
pkgs.fmt_9.overrideAttrs (oldAttrs: {
  # TODO: check version when upgrading wpilib
  version = "8.1.1";
  src = pkgs.fetchFromGitHub {
    owner = "fmtlib";
    repo = "fmt";
    rev = "8.1.1";
    hash = "sha256-leb2800CwdZMJRWF5b1Y9ocK0jXpOX/nwo95icDf308=";
  };
  doCheck = false;
})
