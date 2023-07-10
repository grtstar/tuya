#include <signal.h>
#include <execinfo.h>
#include "utils/log_.h"

#define BACKTRACE_MAX_FRAME_NUMBER 10
/**
 * @brief 崩溃后 dump 调用栈
 * 
 */
class StackTrace
{
public:
    StackTrace()
    {
        signal(SIGSEGV, Dump);
    }
    static void Dump(int signal)
    {
        void *array[BACKTRACE_MAX_FRAME_NUMBER];
        size_t size;
        char **func_name_cache;

        size = backtrace (array, BACKTRACE_MAX_FRAME_NUMBER);
        func_name_cache = backtrace_symbols (array, size);
        // 注意,崩溃时不一定能正常打印
        if(!func_name_cache)
        {
            LOGE("SEGV", "failed to dump stack");
            return;
        }
        LOGE("SEGV", "**************** STACK *******************");
        for (size_t i = 0; i < size; i++) {
            LOGE("SEGV", "{}", func_name_cache[i]);
        }
        LOGE("SEGV", "***************** END ********************");
        free (func_name_cache);
        exit(SIGSEGV);
    }
};