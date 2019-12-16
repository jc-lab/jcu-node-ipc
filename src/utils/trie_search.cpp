//
// Created by jichan onMessage 2019-12-14.
//

#include "trie_search.h"

namespace jcu {
    namespace node_ipc {
        namespace utils {
            void TrieSearchBasic::insert(const std::string& key, std::unique_ptr<WrapperBase> wrapper) {
                size_type key_length = key.length();
                TrieNode *tnode = &root_;
                for(size_type i = 0; i < key_length; i++) {
                    uint8_t c = (uint8_t)key.at(i);
                    if(c == '*') {
                        tnode->wildcarded = std::move(wrapper);
                        break;
                    }

                    tnode = &(tnode->subtree[c]);
                    if(!tnode->node_key) {
                        tnode->node_key = c;
                    }
                }
                tnode->item = std::move(wrapper);
            }

            std::unique_ptr<TrieSearchBasic::WrapperBase>& TrieSearchBasic::ref(const std::string& key) {
                size_type key_length = key.length();
                TrieNode *tnode = &root_;
                for(size_type i = 0; i < key_length; i++) {
                    uint8_t c = (uint8_t)key.at(i);
                    if(c == '*') {
                        return tnode->wildcarded;
                    }

                    tnode = &(tnode->subtree[c]);
                    if(!tnode->node_key) {
                        tnode->node_key = c;
                    }
                }

                return tnode->item;
            }

            bool TrieSearchBasic::remove(const std::string& key) {
                size_type key_length = key.length();
                TrieNode *tnode = &root_;
                TrieNode *pnode = nullptr;
                std::map<uint8_t, TrieNode>::iterator search_iter;
                for(size_type i = 0; i < key_length; i++) {
                    uint8_t c = (uint8_t)key.at(i);
                    if(c == '*') {
                        if(tnode->wildcarded) {
                            tnode->wildcarded.reset();
                            return true;
                        }
                        return false;
                    }
                    search_iter = tnode->subtree.find(c);
                    if(search_iter == tnode->subtree.end()) {
                        return false;
                    }
                    pnode = tnode;
                    tnode = &(search_iter->second);
                }
                pnode->subtree.erase(search_iter);
                return true;
            }

            TrieSearchBasic::WrapperBase* TrieSearchBasic::search(const std::string& key) {
                size_type key_length = key.length();
                TrieNode *tnode = &root_;
                WrapperBase *wildcarded = nullptr;
                for(size_type i = 0; i < key_length; i++) {
                    uint8_t c = (uint8_t)key.at(i);
                    auto search_iter = tnode->subtree.find(c);
                    if(search_iter == tnode->subtree.end()) {
                        if(tnode->wildcarded) {
                            wildcarded = tnode->wildcarded.get();
                        }
                        return wildcarded;
                    }else{
                        if(tnode->wildcarded) {
                            wildcarded = tnode->wildcarded.get();
                        }
                    }
                    tnode = &(search_iter->second);
                }
                return tnode->item.get();
            }
        }
    }
}
