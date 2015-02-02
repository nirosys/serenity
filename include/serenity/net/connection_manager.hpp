#include <set>
#include "connection.hpp"

#ifndef SERENITY_NET_CONNECTION_MANAGER_HPP_
#define SERENITY_NET_CONNECTION_MANAGER_HPP_

namespace serenity { namespace net {

    template <class req_handler>
    class connection_manager {
        public:
            using connection_ptr = std::shared_ptr<connection<req_handler>>;

            connection_manager(const connection_manager &) = delete;
            connection_manager &operator=(const connection_manager &) = delete;

            connection_manager() { }

            void start(connection_ptr);
            void stop(connection_ptr);
            void stop();

            void remove(const connection_ptr &);

        private:
            std::set<connection_ptr> connections_;
    };


    template <class req_handler>
    void connection_manager<req_handler>::start(connection_ptr conn) {
        connections_.insert(conn);
        conn->start();
    }

    template <class req_handler>
    void connection_manager<req_handler>::stop(connection_ptr conn) {
        connections_.erase(conn);
        conn->stop();
    }

    template <class req_handler>
    void connection_manager<req_handler>::stop() {
        for (auto conn : connections_)
            conn->stop();
        connections_.clear();
    }

    template <class req_handler>
    void connection_manager<req_handler>::remove(const connection_ptr &conn) {
        connections_.erase(conn);
    }
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_NET_CONNECTION_MANAGER_HPP_ */
