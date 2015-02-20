# URL Policies

One of the requirements for RESTful services that I had was to implement URL schemes that are standardized
between all of our services. The URL format we are using prefixes the URL with the service name, followed
by the version number (of the form "v[0-9]+", then followed by the endpoint for the service.

Rather than having configurable parsing code that could be altered at runtime (something I do not anticipate
a need for) I figured applying a templated system for defining these schemes as a set of policies, and using
the validation of these policies to key into the services being exposed by serenity, would be a much easier
and favorable implementation.

This spike was my playground into developing that policy implementation.

## Implementation

In this spike a scheme is encoded using a policy\_set specifying the types of policies that will be used
to validate the URL and extract the desired components.

For instance, for the scheme described above (/service/version/endpoint) a policy set could be declared as:

    policy_set<service, version> policy;

This policy set could then be applied to a given URL:

    std::string remaining;
    if (policy.apply("/ekg/v1/summary", remaining)) {
        // Passed policy check
    }
    else {
        // Failed policy check
    }

The contents of `remaining` would be the parts of the URL left after the policy extracted the desired
information. The results of the extraction are then stored within a std::tuple, and can be extracted
via the policy set, using the policy type which extracted the desired information. Such as:

    int v = policy.get<version>();

In the case of an invalid URL, the data returned from `get` depend on which policy failed, and what the
default values for the policy value types are.

In the use case for mapping URLs to services, the resulting tuple will act as a key to determining the
appropriate service.

The `policy_set&lt;service,version&gt;` declares that the service name and version number are required
at the beginning of the URL. These two pieces of information uniquely identify the service that needs to
handle the request. So, the resulting tuple of extracted values will serve as a key into the collection
of versioned services. When adding a new service to the serenity server this tuple will also be generated.


## Limitations

Currently, exceptions have to be used in order to communicate a failure in the policy validation. I'm not
too big on exception usage, but for now I think it is a reasonable requirement. The performance of a RESTful
service (in my experience) can handle the overhead. This, however may change, as it may impact the runtime
of the non-RESTful service code (ie. adding RESTful queries to a process tasked with other responsibilities).

Individual types need to be created for each policy used. I have a generic `string_token_policy` defined,
but it cannot be used directly. A new type would have to derive from it in order to query the value from
the tuple. If the actual value is never desired, then a new type may not be necessary.

## Comments

The implementation here is a little confusing, as the policy should only define the required prefixes to
the URLs. I have in this code a `function` policy type, which can be used to extract the function name of
the endpoint, but that should be left to the rest of the serenity resolvers, and not be a policy check.
