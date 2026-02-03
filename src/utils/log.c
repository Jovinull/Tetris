#include "utils/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static LogLevel g_level = LOG_INFO;

static const char* level_str(LogLevel lv) {
  switch (lv) {
    case LOG_DEBUG: return "DEBUG";
    case LOG_INFO:  return "INFO";
    case LOG_WARN:  return "WARN";
    case LOG_ERROR: return "ERROR";
    default:        return "INFO";
  }
}

void log_set_level(LogLevel level) { g_level = level; }
LogLevel log_get_level(void) { return g_level; }

bool log_is_debug_enabled(void) {
#ifdef TETRIS_NO_DEBUG
  return false;
#else
  return g_level <= LOG_DEBUG;
#endif
}

static void log_v(LogLevel lv, const char* fmt, va_list args) {
  if (lv < g_level) return;

  time_t t = time(NULL);
  struct tm tmv;
#if defined(_POSIX_VERSION)
  localtime_r(&t, &tmv);
#else
  tmv = *localtime(&t);
#endif

  char ts[32];
  (void)snprintf(ts, sizeof(ts), "%02d:%02d:%02d",
                 tmv.tm_hour, tmv.tm_min, tmv.tm_sec);

  FILE* out = (lv >= LOG_WARN) ? stderr : stdout;
  fprintf(out, "[%s] %s: ", ts, level_str(lv));
  vfprintf(out, fmt, args);
  fputc('\n', out);
}

static void log_f(LogLevel lv, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  log_v(lv, fmt, args);
  va_end(args);
}

void log_debug(const char* fmt, ...) {
#ifdef TETRIS_NO_DEBUG
  (void)fmt;
#else
  va_list args;
  va_start(args, fmt);
  log_v(LOG_DEBUG, fmt, args);
  va_end(args);
#endif
}
void log_info(const char* fmt, ...)  { log_f(LOG_INFO, fmt); }
void log_warn(const char* fmt, ...)  { log_f(LOG_WARN, fmt); }
void log_error(const char* fmt, ...) { log_f(LOG_ERROR, fmt); }