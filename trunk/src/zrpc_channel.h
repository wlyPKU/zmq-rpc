/*
 * Copyright(C) lichuang
 */
#ifndef __ZRPC_CHANNEL_H__
#define __ZRPC_CHANNEL_H__
#include <google/protobuf/service.h>
#include <google/protobuf/message.h>
#include <zmq.h>
#include <string>
#include "zrpc_base.h"
namespace zrpc {
class Channel : public gpb::RpcChannel {
public:
 Channel(const string &host, int port);

 virtual ~Channel();

 virtual void CallMethod(const gpb::MethodDescriptor* method,
                         gpb::RpcController* controller,
                         const gpb::Message* request,
                         gpb::Message* response,
                         gpb::Closure* done);

 void Close();
private:
  void *zmq_context_;
  void *zmq_socket_;
  zmq_msg_t zmq_msg_;
};
};
#endif  // __ZRPC_CHANNEL_H__
