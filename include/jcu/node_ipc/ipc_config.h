/**
 * @file	ipc_config.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/12/13
 * @copyright Copyright (C) 2019 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_NODE_IPC_IPC_CONFIG_H__
#define __JCU_NODE_IPC_IPC_CONFIG_H__

#include <string>
#include <functional>
#include <memory>
#include <uvw/loop.hpp>

#include <jcu/transport/ssl_engine.h>

namespace jcu {
    namespace node_ipc {

        typedef std::function<std::shared_ptr<transport::Transport>(std::shared_ptr<uvw::Loop> loop, const std::string& networkHost, int networkPort)> NetworkTransportFactory_t;

        struct IpcTlsConfig {
            std::shared_ptr<transport::SslEngine> engine;

            std::string private_file;
            std::vector<char> private_data;
        };

        struct IpcConfig {
            std::shared_ptr<uvw::Loop> loop;

            /**
             * the id of this socket or service
             */
            std::string id;

            /**
             * the local or remote host on which TCP, TLS or UDP Sockets should connect
             */
            std::string networkHost;

            /**
             * the default port on which TCP, TLS, or UDP sockets should connect
             */
            int networkPort;

            /**
             * this is the time in milliseconds a client will wait before trying to reconnect
             * to a server if the connection is lost. This does not effect UDP sockets since
             * they do not have a client server relationship like Unix Sockets and TCP Sockets.
             */
            int retry;

            NetworkTransportFactory_t network_transport_factory;

            IpcTlsConfig tls;

            IpcConfig() {
                this->networkHost = "localhost";
                this->networkPort = 8000;
                this->retry = 1500;
            }
        };

    }
}

#endif // __JCU_NODE_IPC_IPC_CONFIG_H__
