include_directories(${CGREEN_PUBLIC_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})

# We have two option: build with dl that has GNU api's dlinfo or
# build with libbfd.
find_package(LibBfd)

list(APPEND CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")
list(APPEND CMAKE_REQUIRED_LIBRARIES dl)
check_symbol_exists(dlinfo dlfcn.h HAVE_DLINFO)
list(REMOVE_ITEM CMAKE_REQUIRED_LIBRARIES dl)
list(REMOVE_ITEM CMAKE_REQUIRED_DEFINITIONS "-D_GNU_SOURCE")

if(LibBfd_FOUND OR HAVE_DLINFO)
  set(HAVE_CGREEN_RUNNER 1 CACHE STRING "Has cgreen-runner")

  set(RUNNER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/cgreen-runner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gopt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gopt-errors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/runner.c
    ${CMAKE_CURRENT_SOURCE_DIR}/discoverer_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test_item.c
  )

  # Prefer dlinfo over libbfd
  if(HAVE_DLINFO AND NOT CGREEN_RUNNER_FORCE_LIBBFD)
    list(APPEND RUNNER_SRCS
      ${CMAKE_CURRENT_SOURCE_DIR}/discoverer_dl.c
      ${CMAKE_CURRENT_SOURCE_DIR}/dl_adapter.c
    )
  else()
    list(APPEND RUNNER_SRCS
      ${CMAKE_CURRENT_SOURCE_DIR}/discoverer_bfd.c
      ${CMAKE_CURRENT_SOURCE_DIR}/bfd_adapter.c
    )
  endif()

  set_source_files_properties(${RUNNER_SRCS} PROPERTIES LANGUAGE C)
  include(DefineRelativeFilePaths)
  cmake_define_relative_file_paths ("${RUNNER_SRCS}")

  add_executable(cgreen-runner ${RUNNER_SRCS})
  target_link_libraries(cgreen-runner PRIVATE
    ${CGREEN_SHARED_LIBRARY}
  )

  # Prefer dlinfo over libbfd
  if(HAVE_DLINFO AND NOT CGREEN_RUNNER_FORCE_LIBBFD)
    target_compile_definitions(cgreen-runner PUBLIC _GNU_SOURCE)
    target_link_libraries(cgreen-runner PRIVATE ${CMAKE_DL_LIBS})
  else()
    target_link_libraries(cgreen-runner PRIVATE ${LIBBFD_LIBRARIES})
  endif()

  install(TARGETS cgreen-runner
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  install(PROGRAMS cgreen-debug
    DESTINATION ${CMAKE_INSTALL_BINDIR})
else()
  message(WARNING “libbfd and/or GNU libdl not found, cgreen-runner will not be built”)
endif()
