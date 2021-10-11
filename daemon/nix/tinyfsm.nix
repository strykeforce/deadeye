# nix/tinyfsm.nix
{ sources ? import ./sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.stdenv.mkDerivation rec {
  pname = "tinyfsm";
  version = "0.3.3";

  src = pkgs.fetchFromGitHub {
    owner = "jhh";
    repo = "tinyfsm";
    rev = "v${version}";
    sha256 = "sha256-to9nJG0BDXUwSqsGtID9VpsKjBKPFI+k0Btn75uMplU=";
  };

  nativeBuildInputs = [ pkgs.cmake ];

  cmakeFlags = [
       "-DCMAKE_BUILD_TYPE=Release"
  ];

  outputs = [ "out" ];

  doCheck = false;

  meta = with pkgs.lib; {
    description = "A simple C++ finite state machine library";
    homepage = "https://digint.ch/tinyfsm";
    license = licenses.mit;
  };
}
