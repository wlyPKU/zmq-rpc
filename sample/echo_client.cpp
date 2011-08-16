#include <iostream>
#include <stdio.h>
#include "zrpc_channel.h"
#include "sample/echo.pb.h"
using namespace std;

int main(int argc, char *argv[]) {
  echo::EchoRequest request;
  request.set_message("hello");
  zrpc::Channel rpc_channel("127.0.0.1", 21118);
  echo::EchoService::Stub stub(&rpc_channel);
  echo::EchoResponse response;
  stub.Echo(NULL, &request, &response, NULL);
  printf("receive: %s\n", response.response().c_str());
  rpc_channel.Close();
  return 0;
}

