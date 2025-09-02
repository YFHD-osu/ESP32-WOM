#if LOG_TIMESTAMP
  #define LOG_LOGTAG(NAME, LVL) Serial.printf("[" NAME "][" LVL "][%10lu] ", millis())
#else
  #define LOG_LOGTAG(NAME, LVL) Serial.print("[" NAME "][" LVL "] ")
#endif

#if LOG_LEVEL > 0
  #define LOGE(NAME,...)LOG_LOGTAG("" NAME "","ERROR");Serial.printf(__VA_ARGS__);Serial.println()
#else
  #define LOGE(...) do {} while (0)
#endif

#if LOG_LEVEL > 1
  #define LOGW(NAME,...)LOG_LOGTAG("" NAME ""," WARN");Serial.printf(__VA_ARGS__);Serial.println()
#else
  #define LOGW(...) do {} while (0)
#endif

#if LOG_LEVEL > 2
  #define LOGI(NAME,...)LOG_LOGTAG("" NAME ""," INFO");Serial.printf(__VA_ARGS__);Serial.println()
#else
  #define LOGI(...) do {} while (0)
#endif

#if LOG_LEVEL > 3
  #define LOGD(NAME,...)LOG_LOGTAG("" NAME "","DEBUG");Serial.printf(__VA_ARGS__);Serial.println()
#else
  #define LOGD(...) do {} while (0)
#endif