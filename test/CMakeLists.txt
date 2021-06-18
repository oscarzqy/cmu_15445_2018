set(GMOCK_DIR  "${PROJECT_SOURCE_DIR}/third_party/gmock")
file(GLOB gmock_srcs  ${GMOCK_DIR}/*.cc)
include_directories(SYSTEM ${GMOCK_DIR})
add_library(gtest EXCLUDE_FROM_ALL ${gmock_srcs})
target_link_libraries(gtest ${CMAKE_THREAD_LIBS_INIT})


add_executable(TestHash hash/extendible_hash_test.cpp ../src/hash/extendible_hash.cpp)

set_target_properties(TestHash
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
  )

target_link_libraries(TestHash gtest)