#include "socket/client_socket.h"

using namespace melon::socket;

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../client.log");

  ClientSocket client("127.0.0.1", 7777);

  std::string msg = "hi, I am client!";
  client.send(msg.c_str(), msg.size());

  char buf[1024] = {0};
  client.recv(buf, sizeof(buf));
  log_debug("rec: %s", buf);
  
  client.close();
  return 0;
}
