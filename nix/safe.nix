# nix/safe.nix
{ sources ? import ./sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.stdenv.mkDerivation rec {
  pname = "safe";
  version = "1.0.0";

  src = pkgs.fetchFromGitHub {
    owner = "LouisCharlesC";
    repo = "safe";
    rev = "v${version}";
    sha256 = "sha256-wMCMZ0hSB/2JHhJeb725wsmyTrU1WhAKsfzku81WMl0=";
  };

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
       "-DCMAKE_BUILD_TYPE=Release"
  ];

  outputs = [ "out" ];

  doCheck = false;

  meta = with pkgs.lib; {
    description = "C++11 header only RAII guards for mutexes and locks.";
    homepage = "https://github.com/LouisCharlesC/safe";
    license = licenses.mit;
  };
}
