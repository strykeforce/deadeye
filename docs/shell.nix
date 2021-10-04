{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs.python39Packages; [
    sphinx
    sphinx-autobuild
    sphinx_rtd_theme
  ];
}
