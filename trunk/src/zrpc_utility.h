/*
 * Copyright(C) lichuang
 */
#ifndef __ZRPC_UTILITY_H__
#define __ZRPC_UTILITY_H__
#include <string>
using std::string;
namespace zrpc {
uint32_t hash_string(const string &content);
};
#endif  // __ZRPC_UTILITY_H__
