#include <string>
#include <tuple>
#include <vector>
#include <iostream>
#include <type_traits>
#include <cxxabi.h>

std::string demangle(const char* name) {

    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}


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

template <typename... Types>
struct policy;

template <typename Type, typename... Types>
struct policy<Type, Types...> {
    using type = Type;
    using next = policy<Types...>;
    using value_type = typename return_type<decltype(Type::apply)>::value;
    using tuple_type = std::tuple<value_type, typename policy<Types>::value_type...>;
};

template <>
struct policy<> {
    using type = void;
    using next = void;
    using value_type = void;
    using tuple_type = std::tuple<>;
};

std::string remove_first_uri_element(std::string &uri) {
        std::string::size_type start = (uri[0] == '/') ? 1 : 0;
        std::string::size_type end = uri.find_first_of('/', start);

        if (end == std::string::npos)
            end = uri.size();

        std::string element = uri.substr(start, end - start);
        uri = uri.substr(end);
        return element;
}

struct string_token_policy {
    static std::string apply(std::string &uri) {
        std::string s = remove_first_uri_element(uri);
        return s;
    }
};

struct service_name_policy : public string_token_policy { };
struct function_name_policy : public string_token_policy {};

struct version_policy {
    static int apply(std::string &uri) {
        std::string version = remove_first_uri_element(uri);

        if (version[0] == 'v') {
            int nVer = std::stoi(version.substr(1));
            return nVer;
        }
        return 0;
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
std::tuple<> apply_policies<policy<>>(const std::string &uri, std::string &remaining) {
    return std::tuple<>();
}

template <typename... Policies>
struct policy_list {
    using policies = policy<Policies...>;
    using key_type = std::tuple<typename policy<Policies>::value_type...>;

    key_type values;

    key_type apply(const std::string &uri, std::string &remaining) {
        values = apply_policies<policies>(uri, remaining);

        return values;
    }

    template <typename policy_type>
    typename policy<policy_type>::value_type get() {
        constexpr int i = index_of<policy_type, Policies...>::value;
        typename policy<policy_type>::value_type v = std::get<i>(values);
        return v;
    }
};

int main(int argc, char **argv) {
    using policy_list_type = policy_list<service_name_policy, version_policy>;
    policy_list_type policies;

    std::string remaining;

    typename policy_list_type::key_type t = policies.apply("/test_service/v3/test_function", remaining);
    std::cout << "Remaining URI: " << remaining << std::endl;
    std::cout << "Returned Values:" << std::endl;
    std::cout << "  service name: " << policies.get<service_name_policy>() << std::endl;
    std::cout << "  version     : " << policies.get<version_policy>() << std::endl;

    return 0;
}
