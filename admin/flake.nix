{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    let out = system:
      let pkgs = nixpkgs.legacyPackages."${system}";
      in
      {

        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            python38Packages.poetry
          ];
        };

        packages = {
          default = with pkgs.poetry2nix; mkPoetryApplication {
            projectDir = ./.;
            preferWheels = true;
          };
        };

      }; in with utils.lib; eachSystem defaultSystems out;

}
