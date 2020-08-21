if(NOT trout_JSONCPP_ROOT_DIR)
  set(trout_JSONCPP_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsoncpp)
endif()

if(EXISTS "${trout_JSONCPP_ROOT_DIR}/CMakeLists.txt")
  set(trout_JSONCPP_INCLUDE_DIRS "${trout_JSONCPP_ROOT_DIR}/include")
  set(trout_JSONCPP_LIBRARIES "jsoncpp_lib")
  if (NOT TARGET ${trout_JSONCPP_LIBRARIES})
    add_subdirectory(${trout_JSONCPP_ROOT_DIR} third_party/jsoncpp)
  endif()
else()
    message(FATAL_ERROR "${trout_JSONCPP_ROOT_DIR}/CMakeLists.txt not found")
endif()