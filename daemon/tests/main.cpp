// Copyright (c) 2022 Stryke Force FRC 2767

#define CATCH_CONFIG_RUNNER
#include <networktables/NetworkTableInstance.h>

#include "catch2/catch.hpp"

int main(int argc, char* argv[]) {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::GetDefault();
  nti.StartLocal();

  int result = Catch::Session().run(argc, argv);

  nt::NetworkTableInstance::Destroy(nti);

  return result;
}
