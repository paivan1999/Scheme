if (TEST_SOLUTION)
  file(GLOB SRC "../tests/scheme/*.cpp")
  add_library(libscheme ${SRC})
  target_include_directories(libscheme PUBLIC ../tests/scheme)
else()
  file(GLOB SRC "*.cpp")
  add_library(libscheme ${SRC} object.cpp dead_functions.cpp evaluation.cpp)
  target_include_directories(libscheme PUBLIC .)
endif()

# scheme-repl is not used for testing, but you may use it if you need to experiment with the code.
add_executable(scheme-repl repl/main.cpp)
target_link_libraries(scheme-repl libscheme)

file(GLOB SRC_TEST "test/*.cpp")

add_catch(test_scheme ${SRC_TEST} "test/lsan/disable_lsan.cpp")
target_link_libraries(test_scheme libscheme)

add_catch(test_scheme_no_leaks ${SRC_TEST})
target_link_libraries(test_scheme_no_leaks libscheme)
