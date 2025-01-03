
EventPoller::EventPoller()
  : m_epoll_fd(0)
  , m_max_conn(0)
{
}

EventPoller::~EventPoller()
{
  if (m_epoll_fd > 0)
  {
    ::close(m_epoll_fd);
    m_epoll_fd = 0;
  }

  if (m_events != nullptr)
  {
    delete[] m_events;
    m_events = nullptr;
  }
}

bool EventPoller::create(int max_conn)
{
  m_epoll_fd = ::epoll_create(max_conn);
  if (m_epoll_fd < 0)
    return false;
  
  m_events = new epoll_event[max_conn];
  m_max_conn = max_conn;

  return true;
}

void EventPoller::addFd(int fd, uint32_t events)
{
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  ::epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

void EventPoller::modFd(int fd, uint32_t events)
{
  struct epoll_event ev;
  ev.events = events;
  ev.data.fd = fd;
  ::epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

void EventPoller::delFd(int fd)
{
  ::epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
}

int EventPoller::wait(int timeout)
{
  return ::epoll_wait(m_epoll_fd, m_events, m_max_conn, timeout);
}
 
bool EventPoller::isSet(int idx, uint32_t events)
{
  return m_events[idx].events & events;
}

void EventPoller::getFd(int idx)
{
  return m_events[idx].data.fd;
}

