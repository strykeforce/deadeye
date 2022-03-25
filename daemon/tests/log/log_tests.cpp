// Copyright (c) 2022 Stryke Force FRC 2767

#include <networktables/NetworkTableInstance.h>

#include <catch2/catch.hpp>
#include <nlohmann/json.hpp>

#include "config.h"
#include "log/client_logger.h"

using ::deadeye::CameraControlTablePath;
using ::deadeye::ClientLogger;
using json = nlohmann::json;

namespace {
constexpr std::string_view default_value{"{}"};

json expected(std::string_view level, std::string_view msg) {
  json j;
  j["level"] = level;
  j["message"] = msg;
  return j;
}
}  // namespace

TEST_CASE("client logging updates NetworkTables", "[logging]") {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::Create();
  nti.StartLocal();
  int inum = 1;
  auto table = nti.GetTable(CameraControlTablePath(inum));

  ClientLogger logger{inum, nti};

  std::string_view msg{"test debug message"};
  logger.Debug(msg);
  json logged = json::parse(table->GetString(DE_LOG, default_value));
  REQUIRE(logged == expected("debug", msg));

  msg = "test info message";
  logger.Info(msg);
  logged = json::parse(table->GetString(DE_LOG, default_value));
  REQUIRE(logged == expected("info", msg));

  msg = "test warn message";
  logger.Warn(msg);
  logged = json::parse(table->GetString(DE_LOG, default_value));
  REQUIRE(logged == expected("warn", msg));

  msg = "test error message";
  logger.Error(msg);
  logged = json::parse(table->GetString(DE_LOG, default_value));
  REQUIRE(logged == expected("error", msg));

  nt::NetworkTableInstance::Destroy(nti);
}

TEST_CASE("client logging updates default NetworkTablesInstance", "[logging]") {
  nt::NetworkTableInstance nti = nt::NetworkTableInstance::GetDefault();
  nti.StartLocal();
  int inum = 2;
  auto table = nti.GetTable(CameraControlTablePath(inum));

  ClientLogger logger{inum};
  std::string_view msg{"default nti test"};

  logger.Log(ClientLogger::Level::info, msg);
  json logged = json::parse(table->GetString(DE_LOG, default_value));
  REQUIRE(logged == expected("info", msg));

  nt::NetworkTableInstance::Destroy(nti);
}
