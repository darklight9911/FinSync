#define DEBUG_MODE true
#define OS_NAME "linux"
#if DEBUG_MODE
    #define CONLOG_OUTPUT_ENABLED true
    
#else
    #define CONLOG_OUTPUT_ENABLED false
#endif
#define MAX_SIZE 1000
#define MAX_LINE_LENGTH 256
