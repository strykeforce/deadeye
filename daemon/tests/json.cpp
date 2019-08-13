#include <nlohmann/json.hpp>
#include "catch2/catch.hpp"

using json = nlohmann::json;
using namespace Catch::literals;

TEST_CASE("JSON operations supported", "[json]") {
  auto j = R"(
  {
    "happy": true,
    "pi": 3.141
  }
)"_json;

  REQUIRE(j["happy"]);
  REQUIRE(j["pi"] == 3.141_a);
}
