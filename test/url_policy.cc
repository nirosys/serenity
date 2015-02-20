
#include "catch.hpp"
#include "serenity/serenity.hpp"


namespace tests {

    namespace policies = serenity::http::policies::url;

    template <typename... policies>
    using policy_set = serenity::http::policies::url::policy_set<policies...>;

    TEST_CASE("Verify Policy Validation", "[policy]") {

        SECTION("Should pass version validation") {
            policy_set<policies::version> policy;

            std::string remaining;

            REQUIRE(policy.apply("/v1/test", remaining) == true);
        }

        SECTION("Should fail version validation") {
            policy_set<policies::version> policy;

            std::string remaining;

            REQUIRE(policy.apply("/test", remaining) == false);
            REQUIRE(remaining == "");
        }

        SECTION("Should modify remaining URI properly") {
            policy_set<policies::version> policy;

            std::string remaining;

            REQUIRE(policy.apply("/v1/test", remaining) == true);
            REQUIRE(remaining == "/test");
        }
    }

    TEST_CASE("Verify extraction of data via policy", "[policy]") {
        policy_set<policies::service, policies::version> policy;

        SECTION("Should extract service name and version") {
            std::string remaining;

            REQUIRE(policy.apply("/service/v3/test", remaining) == true);

            std::string service = policy.get<policies::service>();

            REQUIRE(service == "service");

            uint32_t ver = policy.get<policies::version>();

            REQUIRE(ver == 3);
        }
    }

    TEST_CASE("Verify empty policy usage", "[policy]") {
        policy_set<> policy;
        std::string remaining;

        SECTION("Should pass all URIs") {
            REQUIRE(policy.apply("/service/v3/test", remaining));
            REQUIRE(policy.apply("/", remaining));
            REQUIRE(policy.apply("/v3/test/service", remaining));
        }
    }

}

