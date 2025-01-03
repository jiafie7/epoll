#pragma once

#include "socket/socket.h"

namespace melon
{
  namespace socket
  {
    class ClientSocket : public Socket
    {
    public:
      ClientSocket() = delete;
      ClientSocket(const std::string& ip, int port);
      ~ClientSocket() = default;
    };
  }
}
