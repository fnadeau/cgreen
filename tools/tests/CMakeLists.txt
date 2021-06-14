include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CGREEN_PUBLIC_INCLUDE_DIRS}
    ${CURRENT_BINARY_DIR})


set(CGREEN_RUNNER_TESTS_LIBRARY
  cgreen_runner_tests
  CACHE INTERNAL "cgreen-runner tests shared library"
)
set(RUNNER_TESTS_SRCS
  runnerTests.c
  ../test_item.c)
add_library(${CGREEN_RUNNER_TESTS_LIBRARY} SHARED ${RUNNER_TESTS_SRCS})

target_link_libraries(${CGREEN_RUNNER_TESTS_LIBRARY}
  ${CGREEN_SHARED_LIBRARY}
  ${LIBBFD_LIBRARIES})

SET(CGREEN_RUNNER_TESTS_LIBRARY "$<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests>")

# Due to some (of many) CMake irregularities to reference the test libraries
# we can't just use its CMake name variable, but have to look it up with
# some special attributes of the library:
#     $<TARGET_FILE_DIR:the_lib>/$<TARGET_FILE_NAME:the_lib>
#

macro_add_test(NAME cgreen_runner_unit_tests
COMMAND cgreen-runner ${CGREEN_RUNNER_TESTS_LIBRARY})

macro_add_test(NAME cgreen_runner_usage
COMMAND cgreen-runner --help)

macro_add_test(NAME cgreen_runner_quiet
COMMAND cgreen-runner -q ${CGREEN_RUNNER_TESTS_LIBRARY})

macro_add_test(NAME cgreen_runner_verbose
COMMAND cgreen-runner -v -C ${CGREEN_RUNNER_TESTS_LIBRARY})

macro_add_test(NAME cgreen_runner_version
COMMAND cgreen-runner --version)

macro_add_test(NAME cgreen_runner_multiple_libraries
COMMAND cgreen-runner ${CGREEN_RUNNER_TESTS_LIBRARY} ${CGREEN_RUNNER_TESTS_LIBRARY} ${CGREEN_RUNNER_TESTS_LIBRARY})

macro_add_test(NAME cgreen_runner_single_explicit_named_test
COMMAND cgreen-runner $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests> Runner:can_match_test_name)

macro_add_test(NAME cgreen_runner_patternmatched_testnames
COMMAND cgreen-runner $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests> Runner:can*)

macro_add_test(NAME cgreen_runner_patternmatched_testnames_in_patternmatched_context
COMMAND cgreen-runner $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests> Run*:can*)

macro_add_test(NAME cgreen_runner_wildcarded_tests_in_named_context
COMMAND cgreen-runner $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests> Runner:*)

macro_add_test(NAME cgreen_runner_wildcarded_tests_in_wildcarded_context
COMMAND cgreen-runner $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests> *:*)

macro_add_test(NAME cgreen_runner_with_xml_reporter
COMMAND cgreen-runner --xml TEST --suite cgreen_runner_tests $<TARGET_FILE_DIR:cgreen_runner_tests>/$<TARGET_FILE_NAME:cgreen_runner_tests>)

find_package(LibBfd)
if(LibBfd_FOUND)
  set(DISCOVERER_BFD_ACCEPTANCE_TESTS_LIBRARY
    discoverer_bfd_acceptance_tests
    CACHE INTERNAL "discoverer using libbfd acceptance tests"
  )
  set(DISCOVERER_BFD_ACCEPTANCE_TESTS_SRCS
    discoverer_acceptance_tests.c
    ../bfd_adapter.c
    ../discoverer_bfd.c
    ../discoverer_utils.c
    ../test_item.c)
  add_library(${DISCOVERER_BFD_ACCEPTANCE_TESTS_LIBRARY} SHARED ${DISCOVERER_BFD_ACCEPTANCE_TESTS_SRCS})

  target_link_libraries(${DISCOVERER_BFD_ACCEPTANCE_TESTS_LIBRARY}
    ${CGREEN_SHARED_LIBRARY}
    ${LIBBFD_LIBRARIES})

  set(DISCOVERER_BFD_UNIT_TESTS_LIBRARY
    discoverer_bfd_unit_tests
    CACHE INTERNAL "discoverer using libbfd unit tests"
  )
  set(DISCOVERER_BFD_UNIT_TESTS_SRCS
    discoverer_bfd_unit_tests.c
    ../discoverer_bfd.c
    ../discoverer_utils.c)
  add_library(${DISCOVERER_BFD_UNIT_TESTS_LIBRARY} SHARED ${DISCOVERER_BFD_UNIT_TESTS_SRCS})

  target_compile_definitions(${DISCOVERER_BFD_UNIT_TESTS_LIBRARY} PUBLIC UNITTESTING)
  target_link_libraries(${DISCOVERER_BFD_UNIT_TESTS_LIBRARY}
    ${CGREEN_SHARED_LIBRARY}
    ${LIBBFD_LIBRARIES})

  SET(DISCOVERER_BFD_ACCEPTANCE_TESTS_LIBRARY "$<TARGET_FILE_DIR:discoverer_bfd_acceptance_tests>/$<TARGET_FILE_NAME:discoverer_bfd_acceptance_tests>")
  SET(DISCOVERER_BFD_UNIT_TESTS_LIBRARY "$<TARGET_FILE_DIR:discoverer_bfd_unit_tests>/$<TARGET_FILE_NAME:discoverer_bfd_unit_tests>")

  macro_add_test(NAME discoverer_bfd_acceptance_tests
    COMMAND cgreen-runner ${DISCOVERER_BFD_ACCEPTANCE_TESTS_LIBRARY})

  macro_add_test(NAME discoverer_bfd_unit_tests
    COMMAND cgreen-runner ${DISCOVERER_BFD_UNIT_TESTS_LIBRARY})

endif()

if(HAVE_DLINFO)
  set(DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY
    discoverer_dl_acceptance_tests
    CACHE INTERNAL "discoverer using GNU libdl acceptance tests"
  )
  set(DISCOVERER_DL_ACCEPTANCE_TESTS_SRCS
    discoverer_acceptance_tests.c
    ../dl_adapter.c
    ../discoverer_dl.c
    ../discoverer_utils.c
    ../test_item.c)
  add_library(${DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY} SHARED ${DISCOVERER_DL_ACCEPTANCE_TESTS_SRCS})

  target_compile_definitions(${DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY} PUBLIC _GNU_SOURCE)
  target_link_libraries(${DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY}
    ${CGREEN_SHARED_LIBRARY}
    ${CMAKE_DL_LIBS})

  set(DISCOVERER_DL_UNIT_TESTS_LIBRARY
    discoverer_dl_unit_tests
    CACHE INTERNAL "discoverer using libbfd unit tests"
  )
  set(DISCOVERER_DL_UNIT_TESTS_SRCS
    discoverer_dl_unit_tests.c
    ../discoverer_dl.c
    ../discoverer_utils.c)
  add_library(${DISCOVERER_DL_UNIT_TESTS_LIBRARY} SHARED ${DISCOVERER_DL_UNIT_TESTS_SRCS})

  target_compile_definitions(${DISCOVERER_DL_UNIT_TESTS_LIBRARY} PUBLIC UNITTESTING _GNU_SOURCE)
  target_link_libraries(${DISCOVERER_DL_UNIT_TESTS_LIBRARY}
    ${CGREEN_SHARED_LIBRARY}
    ${CMAKE_DL_LIBS})

  SET(DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY "$<TARGET_FILE_DIR:discoverer_dl_acceptance_tests>/$<TARGET_FILE_NAME:discoverer_dl_acceptance_tests>")
  SET(DISCOVERER_DL_UNIT_TESTS_LIBRARY "$<TARGET_FILE_DIR:discoverer_dl_unit_tests>/$<TARGET_FILE_NAME:discoverer_dl_unit_tests>")

  macro_add_test(NAME discoverer_dl_acceptance_tests
    COMMAND cgreen-runner ${DISCOVERER_DL_ACCEPTANCE_TESTS_LIBRARY})

  macro_add_test(NAME discoverer_dl_unit_tests
    COMMAND cgreen-runner ${DISCOVERER_DL_UNIT_TESTS_LIBRARY})

endif()
