{
  description = "Deadeye admin server";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    poetry2nix = {
      url = "github:nix-community/poetry2nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = { self, nixpkgs, flake-utils, poetry2nix }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          inherit (poetry2nix.lib.mkPoetry2Nix { inherit pkgs; }) mkPoetryEnv mkPoetryApplication;
        in
        {
          packages = {
            deadeye-admin = mkPoetryApplication { projectDir = self; };
            default = self.packages.${system}.deadeye-admin;

            venv = mkPoetryEnv {
              projectDir = self;
              groups = [ "main" "dev" ];
            };

            dockerImage = pkgs.dockerTools.buildLayeredImage {
              name = "j3ff/deadeye-admin";
              tag = "latest";

              config = {
                Cmd =
                  let
                    deadeyePkg = self.packages.${system}.deadeye-admin.dependencyEnv;
                  in
                  [ "${deadeyePkg}/bin/deadeye-server" ];
                ExposedPorts = { "5000/tcp" = { }; };
              };
            };
          };

          devShells.default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.deadeye-admin ];
            packages = with pkgs; [ just poetry ];
          };
        });
}
