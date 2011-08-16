/*
 * Copyright(C) lichuang
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <google/protobuf/descriptor.h>
#include "zrpc_channel.h"
#include "zrpc_utility.h"
namespace zrpc {
Channel::Channel(const string &host, int port)
  : zmq_context_(NULL),
    zmq_socket_(NULL) {
  zmq_context_ = zmq_init(1);
  zmq_socket_ = zmq_socket(zmq_context_, ZMQ_REQ);
  char addr[128];
  snprintf(addr, sizeof(addr), "tcp://%s:%d", host.c_str(), port);
  zmq_connect(zmq_socket_, addr);
}

Channel::~Channel() {
  if (zmq_socket_) {
    zmq_close(zmq_socket_);
  }
  if (zmq_context_) {
    zmq_term(zmq_context_);
  }
}

void Channel::CallMethod(const gpb::MethodDescriptor* method,
                         gpb::RpcController* controller,
                         const gpb::Message* request,
                         gpb::Message* response,
                         gpb::Closure* done) {
  uint32_t opcode = htonl(hash_string(method->full_name()));
  size_t msg_len = request->ByteSize() + sizeof(opcode);
  zmq_msg_init_size(&zmq_msg_, msg_len);
  memcpy(zmq_msg_data(&zmq_msg_), &opcode, sizeof(opcode));
  request->SerializeToArray((char*)zmq_msg_data(&zmq_msg_) + sizeof(opcode),
                            msg_len);
  zmq_send(zmq_socket_, &zmq_msg_, 0);
  zmq_msg_close(&zmq_msg_);

  zmq_msg_init(&zmq_msg_);
  zmq_recv(zmq_socket_, &zmq_msg_, 0); 
  response->ParseFromString(((char*)zmq_msg_data(&zmq_msg_)));
  zmq_msg_close(&zmq_msg_);
}

void Channel::Close() {
  zmq_close(zmq_socket_);
  zmq_socket_ = NULL;
  zmq_term(zmq_context_);
  zmq_context_ = NULL;
}
};
