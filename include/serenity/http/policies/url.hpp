#ifndef SERENITY_HTTP_POLICIES_URL_HPP__
#define SERENITY_HTTP_POLICIES_URL_HPP__

#include <tuple>
#include <type_traits>
#include <string>
#include <iostream>

#include "helpers.hpp"

namespace serenity { namespace http { namespace policies {

    namespace url {
        template <typename... Types>
        struct policy;

        template <typename Type, typename... Types>
        struct policy<Type, Types...> {
            using type = Type;
            using next = policy<Types...>;
            using value_type = typename helpers::return_type<decltype(Type::apply)>::value;
            using tuple_type = std::tuple<value_type, typename policy<Types>::value_type...>;
        };

        template <>
        struct policy<> {
            using type = void;
            using next = void;
            using value_type = void;
            using tuple_type = std::tuple<>;
        };

        inline std::string remove_first_uri_element(std::string &uri) {
            std::string::size_type start = (uri[0] == '/') ? 1 : 0;
            std::string::size_type end = uri.find_first_of('/', start);

            if (end == std::string::npos)
                end = uri.size();

            std::string element = uri.substr(start, end - start);
            uri = uri.substr(end);
            return element;
        }

        template <class Policy>
        class policy_exception : public std::exception {
            public:
                policy_exception(const char *wut) : what_(wut) { }

                virtual const char *what() const noexcept override { return what_.c_str(); }
            private:
                const std::string what_;
        };

        // Provided Policies

        struct string_token_policy {
            static std::string apply(std::string &uri) {
                std::string s = remove_first_uri_element(uri);
                if (s.size() == 0)
                    throw policy_exception<string_token_policy>("Token not found.");
                return s;
            }
        };

        // Interprets a token of the URI as the service name.
        struct service : public string_token_policy { };

        // Interprets a token of the URI as an API version (of the form v1, v2, etc)
        struct version {
            static int apply(std::string &uri) {
                bool error = false;
                std::string version = remove_first_uri_element(uri);

                if (version[0] == 'v') {
                    try {
                        int nVer = std::stoi(version.substr(1));
                        return nVer;
                    }
                    catch (std::exception &) {}
                }
                throw policy_exception<url::version>("Invalid version found.");
            }
        };

        template <typename Policyhead>
        typename Policyhead::tuple_type apply_policies(const std::string &uri, std::string &remaining)
        {
            typename Policyhead::type p;
            std::string rem = uri;

            using policy_ret_type = decltype(p.apply(rem));
            policy_ret_type val = p.apply(rem);

            remaining = rem;

            typename Policyhead::next::tuple_type next_tuple =
                apply_policies<typename Policyhead::next>(rem, remaining);

            typename Policyhead::tuple_type ret =
                std::tuple_cat(std::tuple<policy_ret_type>(val), next_tuple);

            return ret;
        }

        template <>
        inline std::tuple<> apply_policies<policy<>>(const std::string &uri, std::string &remaining) {
            remaining = uri;
            return std::tuple<>();
        }

        template <typename... Policies>
        struct policy_set {
            using policies = policy<Policies...>;
            using key_type = std::tuple<typename policy<Policies>::value_type...>;

            key_type values;

            bool apply(const std::string &uri, std::string &remaining) {
                bool valid = true;
                //std::cerr << "[policy] Applying: " << uri << std::endl;
                try {
                    values = apply_policies<policies>(uri, remaining);
                }
                catch (std::exception &ex) {
                    valid = false;
                }

                return valid;
            }

            template <typename policy_type>
            typename policy<policy_type>::value_type get() {
                constexpr int i = helpers::index_of<policy_type, Policies...>::value;
                typename policy<policy_type>::value_type v = std::get<i>(values);
                return v;
            }
        };

        //template <typename policy_type, typename... policy_types>
        //typename policy<policy_type>::value_type get(policy_set<policy_types...> policy) {
        //    constexpr int i = helpers::index_of<policy_type, policy_types...>::value;
        //    typename policy<policy_type>::value_type v = std::get<i>(policy.values);
        //    return v;
        //}
    }
} } }

#endif /* end of include guard: SERENITY_HTTP_POLICIES_URL_HPP__ */
