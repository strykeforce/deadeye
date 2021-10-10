# nix/readerwriterqueue.nix
{ sources ? import ./sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.stdenv.mkDerivation {
  name = "readerwriterqueue";

  src = pkgs.fetchFromGitHub {
    owner = "cameron314";
    repo = "readerwriterqueue";
    rev = "v1.0.5";
    sha256 = "sha256-cPFEl669lEStpWnPHdkTsJx2zXbsmgXexoe/1yFeNkg=";
  };

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
       "-DCMAKE_BUILD_TYPE=Release"
  ];

  outputs = [ "out" ];

  doCheck = false;

  meta = with pkgs.lib; {
    description = "A fast single-producer, single-consumer lock-free queue for C++";
    homepage = "https://github.com/cameron314/readerwriterqueue";
  };
}
