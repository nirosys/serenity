# Example: Basic HTTP

This example shows how to implement a very basic HTTP-based service with
serenity. Example payload can be retrieved at localhost:8084/basic/v1/test

## Setup

The example is split between the basic\_http.cc and basic\_http.hpp files. The
header file includes the service definition(`examples::basic_service`) and
utilizes a lambda expression as the handler. Using `std::bind` would also allow
you to use a method in the service class (or somewhere else) to handle the
request as well.

The `http::service` implementation provides functions for quickly adding handlers
for most of the more useful methods (GET, PUT, POST, DELETE).

The source file is fairly bare-bones. I've setup a namespace alias for the
http::policies namespace, and aliased a specific instance of http::server to
server\_t. The rest of the code is simply adding the service to the server's
resolver, running the server, and waiting for the server to stop.  The stop
can be caused by a well placed ctrl-c.

