{ sources ? import ../nix/sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    nodejs-16_x
    python39Packages.ansible-core
    python39Packages.ansible-lint
    python39Packages.poetry
  ];
}
