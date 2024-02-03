{ lib
, stdenv
, fetchFromGitHub
, cmake
}:
stdenv.mkDerivation rec {
  pname = "readerwriterqueue";
  version = "1.0.5";

  src = fetchFromGitHub {
    owner = "cameron314";
    repo = pname;
    rev = "v${version}";
    sha256 = "sha256-cPFEl669lEStpWnPHdkTsJx2zXbsmgXexoe/1yFeNkg";
  };

  outputs = [ "out" ];

  nativeBuildInputs = [ cmake ];

}


