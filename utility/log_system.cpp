#include "utility/log_system.h"

using namespace melon::utility;

const char* LogSystem::s_log_level[LogSystem::log_count] =  
{
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};
        
// LogSystem* LogSystem::getInstance()
// {
//   if (m_instance == nullptr)
//     m_instance = new LogSystem();
//
//   return m_instance;
// }
//
// LogSystem* LogSystem::m_instance = nullptr;

// LogSystem::LogSystem()
//   : m_log_level(LogSystem::log_debug)
//   , m_len(0)
//   , m_max_size(0)
// {
// }
//
// LogSystem::~LogSystem()
// {
//   close();
// }

void LogSystem::open(const std::string& filename)
{
  m_filename = filename;
  m_fout.open(filename, std::ios::app);
  if (m_fout.fail())
    throw std::logic_error("Open log file failed: " + filename);
  m_fout.seekp(0, std::ios::end);
  m_len = (int)m_fout.tellp();
}

void LogSystem::close()
{
  m_fout.close();
}

void LogSystem::setLevel(LogLevel level)
{
  m_log_level = level;
}

void LogSystem::setMaxSize(int size)
{
  m_max_size = size;
}

void LogSystem::setConsole(bool console)
{
  m_console = console;
}

void LogSystem::log(LogLevel level, const char* filename, int line, const char* format, ...)
{
  if (m_log_level > level)
    return;

  if (m_fout.fail())
    return;
  
  // get current time
  time_t ticks = time(nullptr);
  // format time
  struct tm time_info = { 0 };
  localtime_r(&ticks, &time_info);
  char timestamp[32] = {0};
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &time_info);

  int len = 0;
  // [level] time file:line
  const char* fmt = "[%s]\t %s\t %s:%d\t";
  len = snprintf(nullptr, 0, fmt, s_log_level[level], timestamp, filename, line);
  
  std::ostringstream oss;
  if (len > 0)
  {
    char* buffer = new char[len + 1];
    snprintf(buffer, len + 1, fmt, s_log_level[level], timestamp, filename, line);
    buffer[len] = '\0';
    oss << buffer;
    m_len += len;
    delete[] buffer;
  }
  
  va_list arg_ptr;
  va_start(arg_ptr, format);
  len = vsnprintf(nullptr, 0, format, arg_ptr);
  va_end(arg_ptr);
  
  if (len > 0)
  {
    char* content = new char[len + 1];
    va_start(arg_ptr, format);
    vsnprintf(content, len + 1, format, arg_ptr);
    va_end(arg_ptr);
    content[len] = '\0';
    oss << content;
    m_len += len;
    delete[] content;
  }
  oss << '\n';
  
  const std::string str = oss.str();
  m_fout << str;
  m_fout.flush();

  if (m_console)
    std::cout << str;

  if (m_max_size > 0 && m_len > m_max_size)
    rotateLog();
}

void LogSystem::rotateLog()
{
  close();
  setSleep(1000);

  time_t ticks = time(nullptr);
  struct tm time_info = {0};
  getLocaltime(&time_info, &ticks);
  // localtime_r(&ticks, &time_info);
  char timestamp[32] = {0};
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", &time_info);
  std::string filename = m_filename + timestamp;
  if (rename(m_filename.c_str(), filename.c_str()) != 0)
    throw std::logic_error("Rename log file failed");

  open(m_filename);
}

void LogSystem::setSleep(int milliseconds)
{
#ifdef WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds * 1000);
#endif
}

void LogSystem::getLocaltime(struct tm* time_info, const time_t* ticks)
{
#ifdef WIN32
  localtime_s(time_info, ticks);
#else
  localtime_r(ticks, time_info);
#endif
}




