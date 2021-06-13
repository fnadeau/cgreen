#include "dl_adapter.h"

#include <dlfcn.h>
#include <link.h>
#include <limits.h>
#include <sys/mman.h>

void *dl_adapter_open(const char *filename, int flag)
{
    return dlopen(filename, flag);
}

int dl_adapter_close(void *handle)
{
    return dlclose(handle);
}

int dl_adapter_info(void *handle, int request, void *p)
{
    return dlinfo(handle, request, p);
}
