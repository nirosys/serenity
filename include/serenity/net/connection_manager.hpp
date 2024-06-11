#include <set>
#include "connection.hpp"
#include "serenity/common/request_dispatcher.hpp"

#ifndef SERENITY_NET_CONNECTION_MANAGER_HPP_
#define SERENITY_NET_CONNECTION_MANAGER_HPP_

namespace serenity { namespace net {

    /** \brief Manages all currently active connections to the server.
     *
     * Acts as a container for incoming connections so that they can be tracked
     * and managed.
     */
    template <class service_resolver_type>
    class connection_manager {
        public:
            using connection_ptr = std::shared_ptr<connection<service_resolver_type>>;
            using request = typename service_resolver_type::request;
            using response = typename service_resolver_type::response;

            connection_manager(const connection_manager &) = delete;
            connection_manager &operator=(const connection_manager &) = delete;

            connection_manager() { }

            /** Adds a new connection to the pool, and starts the connection. */
            void start(connection_ptr);
            /** Stops a connection, and removes it from the pool. */
            void stop(connection_ptr);
            /** Stops all connections, and removes them all from the pool. */
            void stop();

            /** Removes a connection from the pool, but does not stop it.
             * \note This should only be called from connection, or if you
             *       REALLY know what you're doing.
             */
            void remove(const connection_ptr &);

        private:
            std::set<connection_ptr> connections_;
            std::mutex mtx;
    };


    template <class service_resolver_type>
    void connection_manager<service_resolver_type>::start(connection_ptr conn) {
        mtx.lock();
        connections_.insert(conn);
        mtx.unlock();
        conn->start();
    }

    template <class service_resolver_type>
    void connection_manager<service_resolver_type>::stop(connection_ptr conn) {
        mtx.lock();
        connections_.erase(conn);
        mtx.unlock();
        conn->stop();
    }

    template <class service_resolver_type>
    void connection_manager<service_resolver_type>::stop() {
        for (auto conn : connections_)
            conn->stop();
        mtx.lock();
        connections_.clear();
        mtx.unlock();
    }

    template <class service_resolver_type>
    void connection_manager<service_resolver_type>::remove(const connection_ptr &conn) {
        mtx.lock();
        connections_.erase(conn);
        mtx.unlock();
    }
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_NET_CONNECTION_MANAGER_HPP_ */
