    
      _________                           .__  __          
     /   _____/ ___________   ____   ____ |__|/  |_ ___.__.
     \_____  \_/ __ \_  __ \_/ __ \ /    \|  \   __<   |  |
     /        \  ___/|  | \/\  ___/|   |  \  ||  |  \___  |
    /_______  /\___  >__|    \___  >___|  /__||__|  / ____|
        \/     \/            \/     \/          \/     


# Serenity
A library for creating RESTful services in C++11 and beyond.


## Purpose
The purpose of Serenity is to provide C++ developers an easy way to create
services that expose REST APIs to clients.

## TODO

    * File Handler
    * Extra URI parts as arguments


## Usage
Serenity is fairly simple to use, by design, requiring only access to the
serenity server, and protocol specific resolvers. Examples of usage can be
found in `examples/` and other more specific examples of usage can be found
in the unit tests (`test/`).

This is a header only library, but using cmake the examples, unit tests, and
spikes can be built. Unit tests can be run via `make run_tests`, or
`make test_report` for jUnit output.

The basic\_http example is a good starting point for getting a RESTful service
embedded into your application.
