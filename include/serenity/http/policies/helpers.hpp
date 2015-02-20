#ifndef SERENITY_HTTP_POLICIES_HELPERS_HPP__
#define SERENITY_HTTP_POLICIES_HELPERS_HPP__

namespace serenity { namespace http { namespace policies {

    namespace helpers {
        template <typename... Types>
        struct index_of {};

        template <typename Policy, typename... Tail>
        struct index_of<Policy, Policy, Tail...> {
            static constexpr std::size_t value = 0;
        };

        template <typename Policy, typename Head, typename... Tail>
        struct index_of<Policy, Head, Tail...> {
            static constexpr std::size_t value = index_of<Policy, Tail...>::value + 1;
        };

        template <typename Policy>
        struct index_of<Policy> {
            static constexpr std::size_t value = 0;
        };

        template <typename... Types>
        struct return_type {};

        template <typename R, typename... Types>
        struct return_type<R(Types...)> {
            using value = R;
        };
    }

} } }

#endif /* end of include guard: SERENITY_HTTP_POLICIES_HELPERS_HPP__ */
