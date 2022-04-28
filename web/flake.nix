{
  description = "Deadeye web front-end";

  inputs.utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
        version = "22.2.0";
        # nodeDependencies = (pkgs.callPackage ./default.nix { }).nodeDependencies;
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "deadeye-web";
          inherit version;
          src = ./.;
          buildInputs = with pkgs; [ nodejs-14_x ];

          phases = "buildPhase";

          #ln -s ${nodeDependencies}/lib/node_modules ./node_modules
          #export PATH="${nodeDependencies}/bin:$PATH"
          #export NODE_PATH="${nodeDependencies}/lib/node_modules"
          buildPhase = ''
            ln -s $src/src ./src
            ln -s $src/public ./public
            ln -s $src/package.json ./package.json
            ln -s $src/craco.config.js ./craco.config.js
            ln -s $src/tsconfig.json ./tsconfig.json

            npm install


            #ls -l node_modules/ > $out
            #cp env-vars $out
            #npm config ls -l > $out
            #npm run build
            #cp -rvf build $out/
          '';
        };

        # devShells.default = with pkgs; mkShell {
        #   packages = [
        #     nodejs-14_x
        #     nodePackages.node2nix
        #   ];
        # };
      });
}
