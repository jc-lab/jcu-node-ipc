#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include <iostream>

#include <jcu/node_ipc/client.h>

#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

#include <uvw/loop.hpp>

#include <jcu/transport/openssl_ssl_engine.h>
#include <jcu/transport/tcp_transport.h>
#include <jcu/transport/tls_transport.h>

#include <jcu/node_ipc/client.h>
#include <jcu/node_ipc/ipc_config.h>

using namespace jcu::transport;

int main() {
    setbuf(stdout, 0);;

    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    std::shared_ptr<uvw::Loop> loop = uvw::Loop::getDefault();

    std::shared_ptr<SslEngine> ssl_engine = OpensslSslEngine::create(TLSv1_2_method());

    auto client = jcu::node_ipc::Client::create();
    jcu::node_ipc::IpcConfig& ipcConfig = client->config();

    ipcConfig.loop = loop;
    // ipcConfig.tls.engine = ssl_engine;

    ipcConfig.networkHost = "127.0.0.1";
    ipcConfig.networkPort = 12321;

    client->onError([](jcu::transport::Error &err, bool &reconnect) -> void {
        printf("ERROR: [reconnect=%d] %d %s\n", reconnect, err.code(), err.what());
    });

    client->onMessage("message", [](Json::Value& data, const std::string& key) -> void {
        printf("DATA: %s: %s\n", key.c_str(), data.asString().c_str());
    });

    client->connectToNet("TEST-IPC", [client]() -> void {
        printf("CONNECTED!!!\n");

        Json::Value doc;
        doc["aaaa"] = 1234;
        doc["cddd"] = 4567;
        client->emit("message", doc);
    });

    loop->run();

    return 0;
}
