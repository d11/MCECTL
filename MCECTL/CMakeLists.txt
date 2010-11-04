cmake_minimum_required (VERSION 2.6)
project (MCECTL)
FILE (GLOB CPPFILES src/*.cpp)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

find_package(BISON)
find_package(FLEX)

include_directories(src)
BISON_TARGET(MyParser src/CTL.ypp ${CMAKE_CURRENT_BINARY_DIR}/CTL_Parse.cc COMPILE_FLAGS -d)
FLEX_TARGET(MyScanner src/CTL.l  ${CMAKE_CURRENT_BINARY_DIR}/CTL_Lex.c COMPILE_FLAGS "--header-file=Lex.h")
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Readline REQUIRED)

include_directories(${READLINE_INCLUDE_DIR})


add_executable(
   MCECTL
   ${CPPFILES}
   ${BISON_MyParser_OUTPUTS}
   ${FLEX_MyScanner_OUTPUTS}
)
target_link_libraries(MCECTL ${READLINE_LIBRARY})
