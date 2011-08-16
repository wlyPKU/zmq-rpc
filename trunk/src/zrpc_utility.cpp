/*
 * Copyright(C) lichuang
 */
#include <arpa/inet.h>
#include "zrpc_utility.h"
extern uint32_t hashword(
    const uint32_t *k, 
    size_t          length,
    uint32_t        initval);
namespace zrpc {
#define HASH_INITVAL 0x31415926
uint32_t hash_string(const string &str) {
  return static_cast<uint32_t>(hashword(
      reinterpret_cast<const uint32_t*>(str.c_str()),
      str.size() / 4, HASH_INITVAL));
}
};
