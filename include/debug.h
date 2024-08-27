#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG 1
#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%03d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)
#define DEBUG_PRINT_NOINFO(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#define DEBUG_PRINT_NOINFO(fmt, args...)
#endif

#endif
