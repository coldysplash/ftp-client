set(target_name ftpclient)

add_library(${target_name} STATIC
  ftpclient/client.cpp
  ftpclient/client.hpp)

include(CompileOptions)
set_compile_options(${target_name})

target_include_directories(
  ${target_name}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
