// A header which will hold common things, that are so small, they don't deserve a separate file
// Just logging macro for now

// LOG_PREFIX has to be defined before the include
#ifndef LOG_PREFIX
#define LOG_PREFIX "[] "
#endif

#define LOG(fmt, ...) printf(LOG_PREFIX fmt, ##__VA_ARGS__)
