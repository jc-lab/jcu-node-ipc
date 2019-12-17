/**
 * @file	client.cpp
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/12/13
 * @copyright Copyright (C) 2019 jc-lab.\n
 *            This software may be modified and distributed under the terms
 *            of the Apache License 2.0.  See the LICENSE file for details.
 */

#include <jcu/node_ipc/client.h>
#include <jcu/node_ipc/ipc_config.h>

#include "utils/trie_search.h"

#include <jcu/transport/tcp_transport.h>
#include <jcu/transport/tls_transport.h>

#include <uvw/timer.hpp>

#include <iostream>
#include <json/json.h>

namespace jcu {
    namespace node_ipc {

        class JsonParseError : public transport::Error {
        public:
            std::string what_;

            JsonParseError(const std::string &what) : what_(what) {}

            const char *what() const override {
                return what_.c_str();
            }
            const char *name() const override {
                return "JsonParseError";
            }
            int code() const override {
                return 0;
            }
            explicit operator bool() const override {
                return true;
            }

        };

        class ClientImpl : public Client {
        private:
            struct MessageCallbackHolder {
                virtual void invoke(Json::Value &data, const std::string& type) = 0;
            };
            struct NormalMessageCallbackHolder : public MessageCallbackHolder {
                OnMessage_t func_;

                NormalMessageCallbackHolder(const OnMessage_t& func) : func_(func) {}

                void invoke(Json::Value &data, const std::string& type) {
                    func_(data);
                }
            };
            struct WithTypeMessageCallbackHolder : public MessageCallbackHolder {
                OnMessageWithType_t func_;

                WithTypeMessageCallbackHolder(const OnMessageWithType_t& func) : func_(func) {}

                void invoke(Json::Value &data, const std::string& type) {
                    func_(data, type);
                }
            };

        public:
            int state_;

            IpcConfig config_;

            typedef std::list<std::unique_ptr<MessageCallbackHolder>> DataHandlerList;

            utils::TrieSearch<DataHandlerList> data_handlers_;

            ErrorCallback_t on_error_;

            std::shared_ptr<transport::Transport> transport_;

            std::unique_ptr<Json::CharReader> json_reader_;

            ClientImpl() {
                Json::CharReaderBuilder reader_builder;
                json_reader_.reset(reader_builder.newCharReader());
                state_ = 0;
            }
            std::shared_ptr<IpcSession> of(const std::string &name) override {
                return std::shared_ptr<IpcSession>();
            }
            void close() {
                std::shared_ptr<transport::Transport> transport = transport_; // .lock();
                state_ = 0;
                if(transport) {
                    transport->cleanup();
                    transport_.reset();
                }
            }
            void onError(ErrorCallback_t on_error) override {
                on_error_ = on_error;
            }
            void connectToNet(const std::string &id,
                              const std::string &host,
                              int port,
                              ConnectCallback_t connect_callback) override {
                std::string conn_id = id.empty() ? config_.id : id;
                std::string conn_host = host.empty() ? config_.networkHost : host;
                int conn_port = (port <= 0) ? config_.networkPort : port;

                std::shared_ptr<uvw::Loop> loop = config_.loop ? config_.loop : uvw::Loop::getDefault();

                std::shared_ptr<transport::Transport> transport;
                if(config_.network_transport_factory) {
                    transport = config_.network_transport_factory(loop, conn_host, conn_port);
                }else{
                    std::shared_ptr<transport::TcpTransport> tcpTransport = transport::TcpTransport::create(loop);
                    tcpTransport->setRemote(conn_host, conn_port);
                    transport = tcpTransport;
                }
                if(config_.tls.engine) {
                    transport = transport::TlsTransport::create(loop, transport, config_.tls.engine);
                }

                state_ = 1;

                transport->onData([this](transport::Transport& transport, std::unique_ptr<char[]> data, size_t length) -> void {
                    char *begin_ptr = data.get();
                    char *end_ptr = begin_ptr + length;
                    char *cur = begin_ptr;

                    while(cur != end_ptr) {
                        if(*cur == 0x0c) {
                            Json::Value doc;
                            std::string err_text;
                            if(json_reader_->parse(begin_ptr, cur, &doc, &err_text)) {
                                std::string type = doc["type"].asString();
                                auto callback = data_handlers_.typedSearch(type);
                                std::cout << "type = " << type << std::endl;
                                if(callback) {
                                    for(auto it = callback->begin(); it != callback->end(); it++) {
                                        (*it)->invoke(doc["data"], type);
                                    }
                                }
                            }else{
                                JsonParseError err(err_text);
                                bool reconnect = false;
                                on_error_(err, reconnect);
                            }
                        }
                        cur++;
                    }
                });
                transport->connect([this, connect_callback](transport::Transport& transport) -> void {
                    // OK
                    state_ = 2;
                    if(connect_callback) {
                        connect_callback();
                    }
                }, [this, loop](transport::Transport& transport) -> void {
                    // Close
                  reconnect();
                }, [this, loop](transport::Transport& transport, transport::Error& err) -> void {
                    bool flag_reconnect = true;
                    if(on_error_) {
                        on_error_(err, flag_reconnect);
                        if(!flag_reconnect) {
                            state_ = 0;
                        }
                    }
                });

                transport_ = transport;
            }
            IpcConfig &config() override {
                return config_;
            }
            SessionAttrBase *getSessionAttr(const std::string &name) override {
                return nullptr;
            }
            const SessionAttrBase *getSessionAttr(const std::string &name) const override {
                return nullptr;
            }
            bool removeSessionAttr(const std::string &name) override {
                return false;
            }
            void setSessionAttr(const std::string &name, std::unique_ptr<SessionAttrBase> session_attr) override {

            }
            void onMessage(const std::string &msg_type, const OnMessage_t& on_message) override {
                DataHandlerList &handler_list = data_handlers_.typedRef(msg_type);
                std::unique_ptr<NormalMessageCallbackHolder> holder(new NormalMessageCallbackHolder(on_message));
                handler_list.emplace_back(std::move(holder));
            }
            void onMessage(const std::string &msg_type, const OnMessageWithType_t& on_message) override {
                DataHandlerList &handler_list = data_handlers_.typedRef(msg_type);
                std::unique_ptr<WithTypeMessageCallbackHolder> holder(new WithTypeMessageCallbackHolder(on_message));
                handler_list.emplace_back(std::move(holder));
            }

            void emit(const std::string& type, Json::Value& data) override {
                Json::FastWriter fast_writer;
                Json::Value doc;
                doc["type"] = type;
                doc["data"] = data;

                std::string output = fast_writer.write(doc);
                size_t out_length = output.length();
                std::unique_ptr<char[]> buf(new char[out_length + 1]);
                memcpy(buf.get(), output.data(), output.length());
                buf.get()[out_length] = 0x0c;
                transport_->write(std::move(buf), out_length + 1);
            }

            void reconnect() {
                std::shared_ptr<uvw::Loop> loop = config_.loop ? config_.loop : uvw::Loop::getDefault();
                std::shared_ptr<transport::Transport> transport = transport_;

                if(state_ > 0) {
                    auto timer = loop->resource<uvw::TimerHandle>();
                    timer->once<uvw::TimerEvent>([transport](uvw::TimerEvent &evt, uvw::TimerHandle& timer) -> void {
                        transport->reconnect();
                    });
                    timer->start(uvw::TimerHandle::Time{config_.retry}, uvw::TimerHandle::Time{0});
                }
            }
        };

        std::shared_ptr<Client> Client::create() {
            std::shared_ptr<Client> client(new ClientImpl());
            return client;
        }

    }
}
