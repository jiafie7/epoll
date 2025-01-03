#pragma once

#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "utility/singleton.h"

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace melon
{
  namespace utility
  {
#define log_debug(format, ...) \
    Singleton<LogSystem>::getInstance()->log(LogSystem::log_debug, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_info(format, ...) \
    Singleton<LogSystem>::getInstance()->log(LogSystem::log_info, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_warn(format, ...) \
    Singleton<LogSystem>::getInstance()->log(LogSystem::log_warn, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_error(format, ...) \
    Singleton<LogSystem>::getInstance()->log(LogSystem::log_error, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_fatal(format, ...) \
    Singleton<LogSystem>::getInstance()->log(LogSystem::log_fatal, __FILE__, __LINE__, format, ##__VA_ARGS__)

    class LogSystem
    {
      SINGLETON(LogSystem);

      public:
        enum LogLevel
        {
          log_debug = 0,
          log_info,
          log_warn,
          log_error,
          log_fatal,
          log_count,
        };
        
        // static LogSystem* getInstance();
        
        void open(const std::string& filename);
        void close();
        
        void setLevel(LogLevel level);
        void setMaxSize(int size);
        void setConsole(bool console);
        void log(LogLevel level, const char* filename, int line, const char* format, ...);


      private:
        // LogSystem();
        // ~LogSystem();

        void rotateLog();
        void getLocaltime(struct tm* time_info, const time_t* ticks);
        void setSleep(int milliseconds);

      private:
        std::string m_filename;
        std::ofstream m_fout; 
        int m_log_level = log_debug;
        int m_len = 0;
        int m_max_size = 0;
        bool m_console = true;
        static const char* s_log_level[log_count];
        
        // static LogSystem* m_instance;
    };
  }
}
