{
  description = "Deadeye develop environment";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs;
  };

  outputs = { self, nixpkgs }:
    {
      devShell = {
        x86_64-linux =
          let
            pkgs = import nixpkgs {
              system = "x86_64-linux";
            };
          in
          pkgs.mkShell {
            buildInputs = with pkgs; [ jdk11 mdbook python39Packages.poetry ];
            shellHook = ''
          '';
          };

        x86_64-darwin =
          let
            pkgs = import nixpkgs {
              system = "x86_64-darwin";
            };
          in
          pkgs.mkShell {
            buildInputs = with pkgs; [ mdbook python39Packages.poetry ];
            shellHook = ''
          '';
          };
      };
    };
}
