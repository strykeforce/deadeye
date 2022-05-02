{
  description = "Webpack Tutorial";

  inputs.utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.n2n.url = "github:jhh/node2nix";

  outputs = { self, nixpkgs, utils, n2n }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs
          {
            inherit system;
            overlays = [ n2n.overlays.${system} ];
          };
        version = "1.0.0";
        nodeDependencies = (pkgs.callPackage ./config/default.nix { }).nodeDependencies;
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "webpack-tutorial";
          inherit version;
          src = ./.;
          buildInputs = with pkgs; [ nodejs-16_x ];

          phases = "buildPhase";

          # ln -s $src/public ./public
          buildPhase = ''
            export PATH="${nodeDependencies}/bin:$PATH"
            export NODE_PATH="${nodeDependencies}/lib/node_modules"
            export NODE_ENV=production
            export BUILD_DIR="$out"
            ln -s $src/src .
            ln -s $src/config .
            ln -s $src/package.json .
            ln -s $src/webpack.config.js .
            ln -s $src/postcss.config.json .
            ln -s $NODE_PATH .

            npm run build
          '';
        };

        devShells.default = with pkgs; mkShell {
          packages = [
            nodejs-16_x
            node2nix
            nodePackages.http-server
          ];
        };
      });
}
