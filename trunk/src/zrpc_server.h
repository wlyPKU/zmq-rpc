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
struct RpcMethod {
 public:
  RpcMethod(gpb::Service *service,
            const gpb::Message *request,
            const gpb::Message *response, 
            const gpb::MethodDescriptor *method)
    : service_(service),
    request_(request),
    response_(response),
    method_(method) {
    }

  gpb::Service *service_;
  const gpb::Message *request_;
  const gpb::Message *response_;
  const gpb::MethodDescriptor *method_;
};

class Server {
 public:
  Server(const string &host, int port);

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
