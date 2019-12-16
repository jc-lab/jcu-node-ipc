/**
 * @file	instance.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/12/13
 * @copyright Copyright (C) 2019 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_NODE_IPC_INSTANCE_H__
#define __JCU_NODE_IPC_INSTANCE_H__

#include "session_attr.h"

#include <string>
#include <functional>

#include <json/value.h>

namespace jcu {
    namespace node_ipc {

        typedef std::function<void(Json::Value&)> OnMessage_t;
        typedef std::function<void(Json::Value&, const std::string& type)> OnMessageWithType_t;

        class IpcConfig;

        class Instance {
        public:
            /**
             * Config reference
             * @return IpcConfig reference
             */
            virtual IpcConfig& config() = 0;

            /**
             * Client Object Scoped Session Attribute
             * @param name
             * @return SessionAttrBase
             */
            virtual SessionAttrBase *getSessionAttr(const std::string &name) = 0;

            /**
             * Client Object Scoped Session Attribute
             * @param name
             * @return SessionAttrBase
             */
            virtual const SessionAttrBase *getSessionAttr(const std::string &name) const = 0;

            /**
             * Client Object Scoped Session Attribute
             * @param name
             * @return True if present, false otherwise.
             */
            virtual bool removeSessionAttr(const std::string& name) = 0;

            /**
             * Client Object Scoped Session Attribute
             * @param name
             */
            virtual void setSessionAttr(const std::string& name, std::unique_ptr<SessionAttrBase> session_attr) = 0;

            virtual void onMessage(const std::string& msg_type, const OnMessage_t& on_message) = 0;
            virtual void onMessage(const std::string& msg_type, const OnMessageWithType_t& on_message) = 0;
        };

    }
}

#endif // __JCU_NODE_IPC_INSTANCE_H__
