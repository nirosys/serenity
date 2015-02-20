#include "url_policies.hpp"
#include <iostream>

namespace policy = serenity::http::policies::url;
using policy_set = policy::policy_set<policy::service, policy::version>;


int main(int argc, char **argv) {
    std::string remaining;
    const std::string url = argv[1];

    policy::policy_set<policy::service, policy::version> policies;
    //policy::policy_set<> policies;

    if (!policies.apply(url, remaining)) {
        std::cout << "Policies failed for url." << std::endl;
    }
    else {
        std::cout << "Policies Applied To: " << url << std::endl;
        std::cout << "Remaining: " << remaining << std::endl;
        std::cout << "Service  : " << policies.get<policy::service>() << std::endl;
        std::cout << "Version  : " << policies.get<policy::version>() << std::endl;
    }

    return 0;
}
