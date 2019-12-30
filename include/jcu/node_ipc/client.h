/**
 * @file	client.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/12/13
 * @copyright Copyright (C) 2019 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_NODE_IPC_CLIENT_H__
#define __JCU_NODE_IPC_CLIENT_H__

#include "instance.h"

#include <memory>
#include <functional>
#include <map>

#include <jcu/transport/error.h>

namespace jcu {
    namespace node_ipc {

        class IpcConfig;
        class IpcSession;

        class Client : public Instance {
        public:
            typedef std::function<void()> ConnectCallback_t;
            typedef std::function<void(transport::Error& err, bool &reconnect)> ErrorCallback_t;

            /**
             * IpcSession
             * @param name
             * @return shared_ptr<IpcSession>
             */
            virtual std::shared_ptr<IpcSession> of(const std::string& name) = 0;

            /**
             * Connect TCP/TLS by network
             * @param name
             */
            virtual void connectToNet(const std::string& id, const std::string& host = "", int port = 0, ConnectCallback_t connect_callback = nullptr) = 0;
            void connectToNet(const std::string& id, int port, ConnectCallback_t connect_callback = nullptr) {
                connectToNet(id, "", port, connect_callback);
            }
            void connectToNet(const std::string& id, ConnectCallback_t connect_callback = nullptr) {
                connectToNet(id, "", 0, connect_callback);
            }

            virtual void close() = 0;

            virtual void onError(ErrorCallback_t on_error) = 0;

            virtual void emit(const std::string& type, const Json::Value& data) = 0;

            static std::shared_ptr<Client> create();
        };

    }
}

#endif // __JCU_NODE_IPC_CLIENT_H__
