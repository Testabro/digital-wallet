cmake_minimum_required(VERSION 3.10)

# Setup compiler
SET(CMAKE_CXX_COMPILER "/usr/bin/g++")

# set the project name and version
project(wallet-service VERSION 0.1)

#specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add boost libraries
SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.42.0 REQUIRED COMPONENTS system thread regex)
IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

SET(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

# set the project name
project(wallet-service)

# add compile options
add_compile_options(-pthread)

# add the executable
add_executable(wallet-service src/ConcreteServiceState.cpp src/Service.cpp src/main.cpp)
TARGET_LINK_LIBRARIES(wallet-service ${USED_LIBS})
