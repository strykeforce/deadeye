{ sources ? import ../nix/sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.mkShell {
  buildInputs = with pkgs; [
    nodejs
  ];
}
