cmake_minimum_required(VERSION 2.6)
project(GLSANDBOX)

if(MSVC)
  include_directories(
    contrib/msvc15-windows-x86/include
    contrib/common
  )

  link_directories(
    ${CMAKE_SOURCE_DIR}/contrib/msvc15-windows-x86/lib
  )
endif()

if(MSVC)
  add_definitions(-DSDL_MAIN_HANDLED)
endif()

add_executable(glsandbox
  src/main.cpp
)

if(MSVC)
  target_link_libraries(glsandbox
    SDL2 glew32 opengl32
  )
endif()
