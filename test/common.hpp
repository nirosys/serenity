#ifndef SERENITY_TEST_COMMON_HPP_
#define SERENITY_TEST_COMMON_HPP_

namespace tests {

    namespace requests {
        // GET
        extern const char *std_get_request;
        extern const char *std_get_versioned_request;
        extern const char *std_get_parameters;

        // POST
        extern const char *std_post_request;

        // Erroneous
        extern const char *incorrect_request_00;
        extern const char *incorrect_request_01;
    }

}

#endif /* end of include guard: SERENITY_TEST_COMMON_HPP_ */
