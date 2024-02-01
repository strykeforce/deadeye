{
  description = "Deadeye admin server";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    poetry2nix = {
      # url = "github:nix-community/poetry2nix";
      url = "github:jhh/poetry2nix/pynetworktables";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, poetry2nix }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        inherit (poetry2nix.lib.mkPoetry2Nix { inherit pkgs; }) mkPoetryEnv mkPoetryApplication;
      in
      {
        packages = {
          deadeye-admin = mkPoetryApplication { projectDir = self; };
          default = self.packages.${system}.deadeye-admin;

          venv = mkPoetryEnv
            {
              projectDir = self;
              groups = [ "main" "dev" ];
            };
        };

        devShells.default = pkgs.mkShell {
          inputsFrom = [ self.packages.${system}.deadeye-admin ];
          packages = [ pkgs.poetry ];
        };
      });
}
