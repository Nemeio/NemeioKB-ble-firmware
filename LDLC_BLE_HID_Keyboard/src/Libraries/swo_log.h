#ifndef SWO_LOG_H
#define SWO_LOG_H

//#define NRF_LOG_PRINTF(...)             SWO_PrintString(__VA_ARGS__) 

void SWO_PrintString(const char *s);
void log_SWO_printf(const char * fmt, ...);
#endif // SWO_LOG_H