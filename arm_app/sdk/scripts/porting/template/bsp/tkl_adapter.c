#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include "tkl_adapter.h"

static void *handle = NULL;

#define LOAD_ADAPT_DEPENDS_LIB(n) do { \
    void *handle; \
    if(strlen(ADAPT_DEPENDS_LIB_##n)) { \
        handle = dlopen(ADAPT_DEPENDS_LIB_##n, RTLD_GLOBAL | RTLD_LAZY); \
        if(handle ==NULL) { \
            fprintf(stderr,"load %s fail\n",ADAPT_DEPENDS_LIB_##n); \
	}} \
    dlerror(); \
}while(0);

static void *tkl_adapt_load_lib(void)
{
    void *handle = NULL;

    if((handle = dlopen(ADAPT_LIB, RTLD_LAZY)) == NULL) {
       fprintf(stderr, "dlopen %s fail:%s\n",ADAPT_LIB,dlerror());
    }

   return handle;
}

void * tkl_adapt_func_get(char *name)
{
    void *sym = NULL;

    if(handle == NULL) {
        if((handle = tkl_adapt_load_lib()) == NULL) {
            fprintf(stderr, "tkl_adapt_load_lib fail\n");
            return NULL;	
	}
    }

    sym = dlsym(handle, name);

    dlerror();

    return sym;
}
