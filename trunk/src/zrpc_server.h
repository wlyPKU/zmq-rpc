/*
 * Copyright(C) lichuang
 */
#ifndef __ZRPC_SERVER_H__
#define __ZRPC_SERVER_H__
#include <arpa/inet.h>
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <zmq.h>
#include <map>
#include <string>
#include "zrpc_base.h"
using std::map;
namespace zrpc {
struct RpcMethod;
class Server {
 public:
  Server(const string &host, int port, int io_threads);

  ~Server();

  void Start();

  void RegisterService(gpb::Service *service);

 private:
  void *zmq_context_;
  void *zmq_socket_;
  zmq_msg_t zmq_msg_;
  typedef map<uint32_t, RpcMethod*> RpcMethodMap;
  RpcMethodMap rpc_method_map_;
};
};
#endif  // __ZRPC_SERVER_H__
