{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    ansible
    nodejs
    python38
    python38Packages.poetry
  ];
}
