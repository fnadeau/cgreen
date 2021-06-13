#include "discoverer.h"

#include <dlfcn.h>
#include <link.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include <cgreen/internal/unit_implementation.h>

#include "../src/utils.h"
#include "dl_adapter.h"
#include "discoverer_utils.h"
#include "test_item.h"

static void add_all_tests_from(ElfW(Sym) *symbols, char *symbol_str, long symbol_size, CgreenVector *tests, bool verbose) {
    int size = symbol_str - (char *)symbols;
    for (int k = 0; k < size / symbol_size; ++k)
    {
        ElfW(Sym) * sym = &symbols[k];
        // If sym is function
        if ((ELF64_ST_TYPE(symbols[k].st_info) == STT_OBJECT)
            && starts_with_cgreen_spec(&symbol_str[sym->st_name]))
        {
            TestItem *test_item = create_test_item_from(&symbol_str[sym->st_name]);
            if (verbose)
                printf("Discovered %s:%s (%s)\n", test_item->context_name, test_item->test_name,
                       test_item->specification_name);
            cgreen_vector_add(tests, test_item);
        }
    }
}

/* Read in the dynamic symbols.  */
static ElfW(Sym) *get_symbols_table(struct link_map *map, long *symbol_size, char **symbol_str)
{
    ElfW(Sym) *symtab = NULL;
    *symbol_size = 0;
    for (ElfW(Dyn) *section = map->l_ld; section->d_tag != DT_NULL; ++section)
    {
        if (section->d_tag == DT_SYMTAB)
        {
            symtab = (ElfW(Sym) *)section->d_un.d_ptr;
        }
        else if (section->d_tag == DT_STRTAB)
        {
            *symbol_str = (char*)section->d_un.d_ptr;
        }
        else if (section->d_tag == DT_SYMENT)
        {
            *symbol_size = section->d_un.d_val;
        }
    }
    return symtab;
}

CgreenVector *discover_tests_in(const char *filename, bool verbose) {
    void *handle = dl_adapter_open(filename, RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) {
        if (verbose)
            printf("%s: dl_adapter_open failed\n", filename);
        return NULL;
    }

    struct link_map *map = NULL;
    int loadInforResult = dl_adapter_info(handle, RTLD_DI_LINKMAP, &map);
    if (loadInforResult == -1) {
        if (verbose)
            printf("%s: dl_atapter_info failed\n", filename);
        dl_adapter_close(handle);
        return NULL;
    }

    ElfW(Sym) *symbols = NULL;
    char *symbol_str = NULL;
    long symbol_size = 0;
    symbols = get_symbols_table(handle, &symbol_size, &symbol_str);

    if ((symbols == NULL) || (symbol_str == NULL) || (symbol_size <= 0)) {
        if (verbose)
            printf("%s: couldn't find any symbol\n", filename);
        dl_adapter_close(handle);
        return NULL;
    }

    CgreenVector *tests = create_cgreen_vector((GenericDestructor)&destroy_test_item);
    add_all_tests_from(symbols, symbol_str, symbol_size, tests, verbose);

    dl_adapter_close(handle);

    return tests;
}
