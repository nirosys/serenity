#include <set>
#include "connection.hpp"

#ifndef SERENITY_NET_CONNECTION_MANAGER_HPP_
#define SERENITY_NET_CONNECTION_MANAGER_HPP_

namespace serenity { namespace net {

    /** \brief Manages all currently active connections to the server.
     *
     * Acts as a container for incoming connections so that they can be tracked
     * and managed.
     */
    template <class request_type, class response_type>
    class connection_manager {
        public:
            using connection_ptr = std::shared_ptr<connection<request_type, response_type>>;
            using request = request_type;
            using response = response_type;

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
    };


    template <class request_type, class response_type>
    void connection_manager<request_type, response_type>::start(connection_ptr conn) {
        connections_.insert(conn);
        conn->start();
    }

    template <class request_type, class response_type>
    void connection_manager<request_type, response_type>::stop(connection_ptr conn) {
        connections_.erase(conn);
        conn->stop();
    }

    template <class request_type, class response_type>
    void connection_manager<request_type, response_type>::stop() {
        for (auto conn : connections_)
            conn->stop();
        connections_.clear();
    }

    template <class request_type, class response_type>
    void connection_manager<request_type, response_type>::remove(const connection_ptr &conn) {
        connections_.erase(conn);
    }
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_NET_CONNECTION_MANAGER_HPP_ */
