#include <sys/epoll.h>
#include "socket/server_socket.h"

using namespace melon::socket;

#define MAX_CONN 1024

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../server.log");

  ServerSocket server("127.0.0.1", 7777);

  int epoll_fd = ::epoll_create(MAX_CONN);
  if (epoll_fd < 0)
  {
    log_error("epoll create error: errno = %d, errmsg = %s.", errno, strerror(errno));
    return -1;
  }

  struct epoll_event ev1;
  ev1.events = EPOLLIN | EPOLLHUP | EPOLLERR;
  ev1.data.fd = server.fd();
  ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server.fd(), &ev1);

  struct epoll_event events[MAX_CONN];

  while (true)
  {
    int ret = ::epoll_wait(epoll_fd, events, MAX_CONN, -1);
    if (ret < 0)
    {
      log_error("epoll wait error: errno = %d, errmsg = %s.", errno, strerror(errno));
      break;
    }
    else if (ret == 0)
    {
      log_debug("epoll wait timeout.");
      continue;
    }
    log_debug("epoll wait ok: ret = %d.", ret);

    for (int i = 0; i < ret; ++ i)
    {
      if (events[i].data.fd == server.fd())
      {
        int conn_fd = server.accept();
        if (conn_fd < 0)
        {
          log_error("server accept error: errno = %d, errmsg = %s.", errno, strerror(errno));
          continue;
        }

        Socket socket(conn_fd);
        socket.setNonBlocking();

        struct epoll_event ev2;
        ev2.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLONESHOT;
        ev2.data.fd = conn_fd;
        ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev2);
      }
      else
      {
        int fd = events[i].data.fd;
        Socket client(fd);

        if (events[i].events & EPOLLHUP)
        {
          log_error("socekt hang up by peer: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
          ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
          client.close();
        }
        else if (events[i].events & EPOLLERR)
        {
          log_error("socekt error: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
          ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
          client.close();
        }
        else if (events[i].events & EPOLLIN)
        {
          ::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
          
          char buf[1024] = {0};
          int len = client.recv(buf, sizeof(buf));

          if (len < 0)
          {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
              log_error("socekt recv would block: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));

              struct epoll_event ev3;
              ev3.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLONESHOT;
              ev3.data.fd = fd;
              ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev3);
            }
            else if (errno == EINTR)
            {
              log_error("socekt recv interrupted: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
              
              struct epoll_event ev3;
              ev3.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLONESHOT;
              ev3.data.fd = fd;
              ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev3);
            }
            log_error("socekt connection abort: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
            client.close();
          }
          else if (len == 0)
          {
            log_debug("socekt closed by peer: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
            client.close();
          }
          else
          {
            log_debug("recv: conn = %d, msg = %s", fd, buf);

            std::string msg = "hi, I am server!";
            client.send(msg.c_str(), msg.size());

            struct epoll_event ev3;
            ev3.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLONESHOT;
            ev3.data.fd = fd;
            ::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev3);
          }
        }
      }
    }
  }

  server.close();

  return 0;
}
