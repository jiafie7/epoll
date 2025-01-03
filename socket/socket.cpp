#include "socket/socket.h"

using namespace melon::socket;

Socket::Socket()
  : m_ip("")
  , m_port(0)
  , m_socket_fd(0)
{
  m_socket_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (m_socket_fd < 0)
    log_error("socket create error: errno = %d, errmsg = %s.", errno, strerror(errno));
  log_debug("socket create success.");
}

Socket::Socket(int socket_fd)
  : m_ip("")
  , m_port(0)
  , m_socket_fd(socket_fd)
{
}

Socket::~Socket()
{
}
 
int Socket::fd() const
{
  return m_socket_fd;
}

bool Socket::bind(const std::string& ip, int port)
{
  struct sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  if (ip.empty())
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  else
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
  sockaddr.sin_port = htons(port);

  if (::bind(m_socket_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
  {
    log_error("socket bind error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  log_debug("socket bind success.");

  m_ip = ip;
  m_port = port;
  return true;
}   

bool Socket::listen(int backlog)
{
  if (::listen(m_socket_fd, backlog) < 0)
  {
    log_error("socket bind error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  log_debug("socket start listening ...");

  return true;
}

bool Socket::connect(const std::string& ip, int port)
{
  struct sockaddr_in sockaddr;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());
  sockaddr.sin_port = htons(port);

  if (::connect(m_socket_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
  {
    log_error("socket connect error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  
  m_ip = ip;
  m_port = port;
  return true;
}

int Socket::accept()
{
  int conn_fd = ::accept(m_socket_fd, nullptr, nullptr);
  if (conn_fd < 0)
  {
    log_error("socket accept error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return -1;
  }

  log_debug("socket accept: conn_fd = %d.", conn_fd);
  return conn_fd;
}  

int Socket::send(const char* buf, int len)
{
  return ::send(m_socket_fd, buf, len, 0);
}

int Socket::recv(char* buf, int len)
{
  return ::recv(m_socket_fd, buf, len, 0);
}

void Socket::close()
{
  if (m_socket_fd > 0)
  {
    ::close(m_socket_fd);
    m_socket_fd = 0;
  }
}

bool Socket::setNonBlocking()
{
  int flags = fcntl(m_socket_fd, F_GETFL, 0);
  if (flags < 0)
  {
    log_error("socket set non blocking error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }

  flags |= O_NONBLOCK;
  if (fcntl(m_socket_fd, F_SETFL, flags) < 0)
  {
    log_error("socket set non blocking error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Socket::setSendBuffer(int size)
{
  int buffer_size = size;
  if (setsockopt(m_socket_fd, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size)) < 0)
  {
    log_error("socket set send buffer error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Socket::setRecvBuffer(int size)
{
  int buffer_size = size;
  if (setsockopt(m_socket_fd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0)
  {
    log_error("socket set recv buffer error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Socket::setLinger(bool active, int seconds)
{
  struct linger lg;
  std::memset(&lg, 0, sizeof(lg));
  lg.l_onoff = active ? 1 : 0;
  lg.l_linger = seconds;
  if (setsockopt(m_socket_fd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg)) < 0)
  {
    log_error("socket set linger error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Socket::setKeepAlive()
{
  int flag = 1;
  if (setsockopt(m_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag)) < 0)
  {
    log_error("socket set keep alive error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Socket::setReuseAddr()
{
  int flag = 1;
  if (setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0)
  {
    log_error("socket set reuse addr error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return false;
  }
  return true;
}

