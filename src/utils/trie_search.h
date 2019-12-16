//
// Created by jichan onMessage 2019-12-14.
//

#ifndef __SRC_UTILS_TRIE_SEARCH_H__
#define __SRC_UTILS_TRIE_SEARCH_H__

#include <string>
#include <memory>
#include <map>
#include <list>

namespace jcu {
    namespace node_ipc {
        namespace utils {

            class TrieSearchBasic {
            public:
                typedef uint64_t size_type;

                struct WrapperBase {
                    virtual ~WrapperBase() {}
                };
                template<typename V>
                struct Wrapper : public WrapperBase {
                    V data;
                };

            protected:
                struct TrieNode {
                    uint8_t node_key;
                    std::unique_ptr<WrapperBase> item;
                    std::unique_ptr<WrapperBase> wildcarded;
                    std::map<uint8_t, TrieNode> subtree;
                };

                TrieNode root_;

            public:
                void insert(const std::string& key, std::unique_ptr<WrapperBase> wrapper);
                std::unique_ptr<WrapperBase>& ref(const std::string& key);
                bool remove(const std::string& key);

                WrapperBase* search(const std::string& key);
            };

            template<typename V>
            class TrieSearch : public TrieSearchBasic {
            public:
                void insert(const std::string& key, V&& value) {
                    std::unique_ptr<Wrapper<V>> wrapper(new Wrapper<V>());
                    wrapper->data = std::forward<V>(value);
                    TrieSearchBasic::insert(key, std::move(wrapper));
                }
                V& typedRef(const std::string& key) {
                    std::unique_ptr<TrieSearchBasic::WrapperBase>& r = this->ref(key);
                    if(!r) {
                        r.reset(new Wrapper<V>());
                    }
                    Wrapper<V> *wrapper = (Wrapper<V>*)r.get();
                    return wrapper->data;
                }

                V* typedSearch(const std::string& key) {
                    Wrapper<V> *wrapper = (Wrapper<V>*)this->search(key);
                    if(!wrapper)
                        return nullptr;
                    return &wrapper->data;
                }
            };

        }
    }
}

#endif //__SRC_UTILS_TRIE_SEARCH_H__
