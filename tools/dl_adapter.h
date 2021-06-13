#ifndef DL_ADAPTER_H
#define DL_ADAPTER_H

#include <dlfcn.h>

#ifdef UNITTESTING
#define dl_adapter_open dl_adapter_open_unittesting
#define dl_adapter_close dl_adapter_close_unittesting
#define dl_adapter_info dl_adapter_info_unittesting
#endif

void *dl_adapter_open(const char *filename, int flag);
int dl_adapter_close(void *handle);

int dl_adapter_info(void *handle, int request, void *p);

#endif
