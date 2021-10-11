# nix/wpilib.nix
{ sources ? import ./sources.nix }:
let
  pkgs = import sources.nixpkgs { };
in
pkgs.stdenv.mkDerivation rec {
  pname = "wpilib";
  version = "2021.3.1";

  src = pkgs.fetchFromGitHub {
    owner = "wpilibsuite";
    repo = "allwpilib";
    rev = "v${version}";
    sha256 = "sha256-nGSegPneo/MDsEuz59E47Gnvz1JQ8Oro5NvimYQNCWM=";
  };

  nativeBuildInputs = [ pkgs.cmake ];
  buildInputs = [ pkgs.gcc-unwrapped ];
  propagatedBuildInputs = [ pkgs.opencv4 ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DWITH_JAVA=OFF"
    "-DWITH_CSCORE=ON"
    "-DWITH_WPIMATH=OFF"
    "-DWITH_WPILIB=OFF"
    "-DWITH_OLD_COMMANDS=OFF"
    "-DWITH_EXAMPLES=OFF"
    "-DWITH_TESTS=OFF"
    "-DWITH_GUI=OFF"
    "-DWITH_SIMULATION_MODULES=OFF"
  ];

  outputs = [ "out" ];

  doCheck = true;
  preCheck = "export LD_LIBRARY_PATH=$(pwd):$LD_LIBRARY_PATH";

  meta = with pkgs.lib; {
    description = "WPILib FRC Robot library";
    homepage = "https://wpilib.org";
    license = licenses.bsd3;
  };
}
