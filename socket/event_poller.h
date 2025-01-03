#pragma once

#include <sys/epoll.h>

namespace melon
{
  namespace socket
  {
    class EventPoller
    {
    public:
      EventPoller();
      ~EventPoller();

      bool create(int max_conn);
      void addFd(int fd, uint32_t events);
      void modFd(int fd, uint32_t events);
      void delFd(int fd);

      int wait(int timeout);

      bool isSet(int idx, uint32_t events);
      void getFd(int idx);

    private:
      int m_epoll_fd;
      int m_max_conn;
      struct epoll_event* m_events;
    };
  }
}
