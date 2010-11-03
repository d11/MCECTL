cmake_minimum_required (VERSION 2.6)
project (MCECTL)
FILE (GLOB CPPFILES src/*.cpp)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

find_package(BISON)
find_package(FLEX)

include_directories(src)
BISON_TARGET(MyParser src/CTL.ypp ${CMAKE_CURRENT_BINARY_DIR}/CTL_Parse.cc COMPILE_FLAGS -d)
FLEX_TARGET(MyScanner src/CTL.l  ${CMAKE_CURRENT_BINARY_DIR}/CTL_Lex.c)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(
   MCECTL
   ${CPPFILES}
   ${BISON_MyParser_OUTPUTS}
   ${FLEX_MyScanner_OUTPUTS}
)
