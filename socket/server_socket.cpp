#include "socket/server_socket.h"

using namespace melon::socket;

ServerSocket::ServerSocket(const std::string& ip, int port)
{
  // setNonBlocking();
  setSendBuffer(10 * 1024);
  setRecvBuffer(10 * 1024);
  setLinger(true, 0);
  setKeepAlive();
  setReuseAddr();
  bind(ip, port);
  listen(1024);
}

