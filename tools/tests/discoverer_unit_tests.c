#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "discoverer.h"
#include "test_item.h"

#include <link.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "dl_adapter.h"
#include "dl_adapter.mocks"

int printf_unittesting(const char *fmt, ...) {
    return (int) mock(fmt);
}

Describe(Discoverer);
BeforeEach(Discoverer) {}
AfterEach(Discoverer) {}

static bool verbose = true;

/* Helper functions */
static void expect_open_file(const char *filename, void *result) {
    expect(dl_adapter_open,
           when(filename, is_equal_to_string(filename)),
           will_return(result));
}

/*======================================================================*/
Ensure(Discoverer, should_find_no_tests_in_non_existing_file) {
    expect_open_file("non-existing-file", NULL);
    expect(printf_unittesting);

    CgreenVector *tests = discover_tests_in("non-existing-file", verbose);

    assert_that(tests, is_null);
}

/*======================================================================*/
Ensure(Discoverer, should_find_no_tests_if_dlinfo_returns_minus_one) {
    struct link_map handle;
    expect_open_file("a-file.so", &handle);
    expect(dl_adapter_info,
           when(handle, is_equal_to(&handle)),
           will_return(-1));
    expect(printf_unittesting);
    expect(dl_adapter_close,
           when(handle, is_equal_to(&handle)));

    CgreenVector *tests = discover_tests_in("a-file.so", verbose);

    assert_that(tests, is_null);
}

/*======================================================================*/
Ensure(Discoverer, should_find_no_tests_if_dynamic_section_is_empty) {
    struct link_map handle;
    ElfW(Dyn) ld;
    ld.d_tag = DT_NULL;
    handle.l_ld = &ld;
    expect_open_file("a-file.so", &handle);
    expect(dl_adapter_info,
           when(handle, is_equal_to(&handle)),
           will_return(0));
    expect(printf_unittesting);
    expect(dl_adapter_close,
           when(handle, is_equal_to(&handle)));

    CgreenVector *tests = discover_tests_in("a-file.so", verbose);

    assert_that(tests, is_null);
}

/*======================================================================*/
Ensure(Discoverer, should_find_no_tests_if_testpattern_is_not_found) {
    const char sym_str_string[] = "NotCgreenSpec__Context1__test1__\0NotCgreenSpec__Context1__test2__\0NotCgreenSpec__Context1__test3__";
    struct link_map handle;
    ElfW(Dyn) ld[4];
    ElfW(Sym) *sym = malloc(sizeof(ElfW(Sym)) * 3);
    char *sym_str = malloc(sizeof(char) * 256);
    handle.l_ld = ld;
    ld[0].d_tag = DT_SYMTAB;
    ld[0].d_un.d_ptr = (Elf64_Xword)sym;
    sym[0].st_info = STT_OBJECT;
    sym[0].st_name = 0;
    sym[1].st_info = STT_OBJECT;
    sym[1].st_name = 33;
    sym[2].st_info = STT_OBJECT;
    sym[2].st_name = 66;
    ld[1].d_tag = DT_STRTAB;
    memcpy(sym_str, sym_str_string, sizeof(sym_str_string));
    ld[1].d_un.d_ptr = (Elf64_Xword)sym_str;
    ld[2].d_tag = DT_SYMENT;
    ld[2].d_un.d_val = sizeof(ElfW(Sym));
    ld[3].d_tag = DT_NULL;

    expect_open_file("a-file.so", &handle);
    expect(dl_adapter_info,
           when(handle, is_equal_to(&handle)),
           will_return(0));
    expect(dl_adapter_close,
           when(handle, is_equal_to(&handle)));

    CgreenVector *tests = discover_tests_in("a-file.so", verbose);

    assert_that(cgreen_vector_size(tests), is_equal_to(0));

    free(sym);
    free(sym_str);
}

/*======================================================================*/
Ensure(Discoverer, should_find_tests_if_test_pattern_is_present) {
    const char sym_str_string[] = "CgreenSpec__Context1__test1__\0CgreenSpec__Context1__test2__\0CgreenSpec__Context1__test3__";
    struct link_map handle;
    ElfW(Dyn) ld[4];
    ElfW(Sym) *sym = malloc(sizeof(ElfW(Sym)) * 3);
    char *sym_str = malloc(sizeof(char) * 256);
    handle.l_ld = ld;
    ld[0].d_tag = DT_SYMTAB;
    ld[0].d_un.d_ptr = (Elf64_Xword)sym;
    sym[0].st_info = STT_OBJECT;
    sym[0].st_name = 0;
    sym[1].st_info = STT_OBJECT;
    sym[1].st_name = 30;
    sym[2].st_info = STT_OBJECT;
    sym[2].st_name = 60;
    ld[1].d_tag = DT_STRTAB;
    memcpy(sym_str, sym_str_string, sizeof(sym_str_string));
    ld[1].d_un.d_ptr = (Elf64_Xword)sym_str;
    ld[2].d_tag = DT_SYMENT;
    ld[2].d_un.d_val = sizeof(ElfW(Sym));
    ld[3].d_tag = DT_NULL;

    expect_open_file("a-file.so", &handle);
    expect(dl_adapter_info,
           when(handle, is_equal_to(&handle)),
           will_return(0));
    expect(printf_unittesting, times(3));
    expect(dl_adapter_close,
           when(handle, is_equal_to(&handle)));

    CgreenVector *tests = discover_tests_in("a-file.so", verbose);

    assert_that(cgreen_vector_size(tests), is_equal_to(3));

    free(sym);
    free(sym_str);
}
