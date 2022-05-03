{
  description = "Deadeye provisioning and deployment Ansible playbooks";

  inputs.utils.url = "github:numtide/flake-utils";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs";
  inputs.deadeye-admin.url = "path:../admin";
  inputs.deadeye-web.url = "path:../web";

  outputs = { self, nixpkgs, utils, deadeye-admin, deadeye-web }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = with pkgs; mkShell {
          packages = [
            ansible
            ansible-lint
          ]
          ++ deadeye-admin.devShells.${system}.default.nativeBuildInputs
          ++ deadeye-web.devShells.${system}.default.nativeBuildInputs;
        };
      });
}
