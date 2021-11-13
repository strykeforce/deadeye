{
  description = "Deadeye develop environment";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs;
  };

  outputs = { self, nixpkgs }:
    let
      pkgs = import nixpkgs {
        system = "x86_64-linux";
      };
    in
    {

      devShell.x86_64-linux = pkgs.mkShell {
        buildInputs = with pkgs; [ jdk11 mdbook python39Packages.poetry ];
        shellHook = ''
          '';
      };
    };
}
