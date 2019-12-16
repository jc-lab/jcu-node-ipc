/**
 * @file	session-attr.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/12/13
 * @copyright Copyright (C) 2019 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#ifndef __JCU_NODE_IPC_SESSION_ATTR_H__
#define __JCU_NODE_IPC_SESSION_ATTR_H__

#include <memory>
#include <functional>
#include <map>
#include <string>

namespace jcu {
    namespace node_ipc {
        enum SessionAttrType {
            SESSION_ATTR_UNKNOWN = 0,
            SESSION_ATTR_UNIQUE_PTR,
            SESSION_ATTR_SHARED_PTR,
            SESSION_ATTR_WEAK_PTR,
            SESSION_ATTR_USER,
        };

        enum SessionAttrScope {
            SESS_ATTR_CLIENT_SCOPE = 0,
            SESS_ATTR_CONNECTION_SCOPE = 1,
        };

        class SessionAttrBase {
        public:
            virtual SessionAttrType getType() const = 0;
            virtual const void *get() const = 0;
            virtual void *get() = 0;
            virtual void clear() = 0;
        };

        template<class S>
        class SessionAttr;

        template<class T>
        class SessionAttr<std::unique_ptr<T>> : public SessionAttrBase {
        private:
            std::unique_ptr<T> data_;

        public:
            SessionAttr(std::unique_ptr<T> data) {
                data_ = std::move(data);
            }
            SessionAttrType getType() const override {
                return SESSION_ATTR_UNIQUE_PTR;
            }
            const void *get() const override {
                return data_.get();
            }
            void *get() override {
                return data_.get();
            }
            void clear() override {
                data_.reset(nullptr);
            }
            std::unique_ptr<T> &refUniquePtr() {
                return data_;
            }
        };

        template<class T>
        class SessionAttr<std::shared_ptr<T>> : public SessionAttrBase {
        private:
            std::shared_ptr<T> data_;

        public:
            SessionAttr(std::shared_ptr<T> data) {
                data_ = data;
            }
            SessionAttrType getType() const override {
                return SESSION_ATTR_SHARED_PTR;
            }
            const void *get() const override {
                return data_.get();
            }
            void *get() override {
                return data_.get();
            }
            void clear() override {
                data_.reset();
            }
            std::shared_ptr<T> &refSharedPtr() {
                return data_;
            }
        };

        template<class T>
        class SessionAttr<std::weak_ptr<T>> : public SessionAttrBase {
        private:
            std::weak_ptr<T> data_;

        public:
            SessionAttr(std::weak_ptr<T> data) {
                data_ = data;
            }
            SessionAttrType getType() const override {
                return SESSION_ATTR_WEAK_PTR;
            }
            const void *get() const override {
                return data_.get();
            }
            void *get() override {
                return data_.lock().get();
            }
            void clear() override {
                data_.reset();
            }
            std::weak_ptr<T> &redWeakPtr() {
                return data_;
            }
            std::shared_ptr<T> lock() {
                return data_.lock();
            }
        };
    }
}

#endif // __JCU_NODE_IPC_SESSION_ATTR_H__
