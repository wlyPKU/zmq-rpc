/*
 * Copyright(C) lichuang
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>
#include "zrpc_server.h"
#include "zrpc_utility.h"
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

Server::Server(const string &host, int port, int io_threads)
  : zmq_context_(NULL),
    zmq_socket_(NULL) {
  zmq_context_ = zmq_init(io_threads);
  zmq_socket_ = zmq_socket(zmq_context_, ZMQ_REP);
  char addr[128];
  snprintf(addr, sizeof(addr), "tcp://%s:%d", host.c_str(), port);
  zmq_bind(zmq_socket_, addr);
}

Server::~Server() {
  if (zmq_socket_) {
    zmq_close(zmq_socket_);
  }
  if (zmq_context_) {
    zmq_term(zmq_context_);
  }
  RpcMethodMap::iterator iter;
  for (iter = rpc_method_map_.begin(); iter != rpc_method_map_.end();) {
    RpcMethod *rpc_method = iter->second;
    ++iter;
    delete rpc_method;
  }
}

void Server::RegisterService(gpb::Service *service) {
  const gpb::ServiceDescriptor *descriptor = service->GetDescriptor();
  for (int i = 0; i < descriptor->method_count(); ++i) {
    const gpb::MethodDescriptor *method = descriptor->method(i);
    const google::protobuf::Message *request = 
      &service->GetRequestPrototype(method);
    const google::protobuf::Message *response =
      &service->GetResponsePrototype(method);
    RpcMethod *rpc_method = new RpcMethod(service, request,
                                          response, method);
    uint32_t opcode = hash_string(method->full_name());
    rpc_method_map_[opcode] = rpc_method;
  }   
}

void Server::Start() {
  uint32_t opcode = 0;
  while (1) { 
    zmq_msg_init(&zmq_msg_);
    zmq_recv(zmq_socket_, &zmq_msg_, 0);  // 0 means no flags, block recv
    memcpy((char*)(&opcode), zmq_msg_data(&zmq_msg_), sizeof(opcode));
    opcode = ntohl(opcode);
    RpcMethodMap::const_iterator iter = rpc_method_map_.find(opcode);
    if (iter == rpc_method_map_.end()) {
      continue;
    }
    RpcMethod *rpc_method = iter->second;
    const gpb::MethodDescriptor *method = rpc_method->method_;
    gpb::Message *request = rpc_method->request_->New();
    gpb::Message *response = rpc_method->response_->New();
    request->ParseFromString(((char*)zmq_msg_data(&zmq_msg_)) + sizeof(opcode));
    zmq_msg_close(&zmq_msg_); 
    rpc_method->service_->CallMethod(method,
                                     NULL,
                                     request, response, NULL);
        
    size_t msg_len = response->ByteSize();
    zmq_msg_init_size(&zmq_msg_, msg_len);
    response->SerializeToArray((char*)zmq_msg_data(&zmq_msg_), msg_len);
    zmq_send(zmq_socket_, &zmq_msg_, 0);
    zmq_msg_close(&zmq_msg_);
    delete request;
    delete response;
  } 
}
};
