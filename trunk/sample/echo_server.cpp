#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <iostream>
#include "zrpc_server.h"
#include "echo.pb.h"

using namespace zrpc;
using namespace std;

class EchoServiceImpl : public echo::EchoService {
 public:
  EchoServiceImpl() {
  };

  virtual void Echo(::google::protobuf::RpcController* controller,
                    const ::echo::EchoRequest* request,
                    ::echo::EchoResponse* response,
                    ::google::protobuf::Closure* done) {
    printf("receive: %s\n", request->message().c_str());
    response->set_response(string("server: ") + request->message());
    if (done) {
      done->Run();
    }
  }
};

int main(int argc, char *argv[]) {
  Server rpc_server("127.0.0.1", 21118, 1);
  gpb::Service *service = new EchoServiceImpl();
  rpc_server.RegisterService(service);
  rpc_server.Start();
  return 0;
}
