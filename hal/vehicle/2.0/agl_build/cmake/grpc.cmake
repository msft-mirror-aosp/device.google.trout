if(NOT trout_GRPC_ROOT_DIR)
  set(trout_GRPC_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/grpc)
endif()

if(EXISTS "${trout_GRPC_ROOT_DIR}/CMakeLists.txt")
  add_subdirectory(${trout_GRPC_ROOT_DIR})
  if(TARGET ${_trout_GRPCXX_LIBRARY_NAME})
    set(trout_GRPCXX_LIBRARIES ${_trout_GRPCXX_LIBRARY_NAME})
  endif()
else()
  message(FATAL_ERROR "${trout_GRPC_ROOT_DIR}/CMakeLists.txt not found")
endif()