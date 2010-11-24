cmake_minimum_required (VERSION 2.8.1)
project (MCECTL)
cmake_policy(VERSION 2.8.1)

#SET(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(CMAKE_CXX_FLAGS "-g -Wall")
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "Lex.h;CTL_Parse.output")
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# BISON/FLEX
find_package(BISON)
find_package(FLEX)
BISON_TARGET(MyParser src/CTL.ypp ${CMAKE_CURRENT_BINARY_DIR}/CTL_Parse.cc COMPILE_FLAGS "-d -v")
FLEX_TARGET(MyScanner src/CTL.l  ${CMAKE_CURRENT_BINARY_DIR}/CTL_Lex.cc COMPILE_FLAGS "--header-file=Lex.h")
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

# READLINE
find_package(Readline REQUIRED)
include_directories(${READLINE_INCLUDE_DIR})

# MAIN PROGRAM
#FILE (GLOB_RECURSE CPPFILES src/*.cpp)
set(CPPFILES
   src/ModelChecker.cpp
   src/formula/Formula.cpp
   src/Environment.cpp
	src/AST/Formula.cpp
	src/AST/Regex.cpp
   src/TransitionSystem.cpp
   src/Automata.cpp
   src/Util.cpp
)


#SET(CMAKE_C_FLAGS "-std=c99")
#add_library(
#   fa
#   lib/libfa/fa.c
#   lib/libfa/hash.c
#   lib/libfa/internal.c
#   lib/libfa/memory.c
#   lib/libfa/ref.c
#   lib/libfa/augeas.c
#)

add_library(
   MCECTL STATIC
   ${CPPFILES}
   ${BISON_MyParser_OUTPUTS}
   ${FLEX_MyScanner_OUTPUTS}
)
link_directories("lib/libfa")
target_link_libraries(MCECTL fa)

add_executable(
   MCECTL-REPL
   src/MCECTL.cpp
)
target_link_libraries(MCECTL-REPL ${READLINE_LIBRARY})
target_link_libraries(MCECTL-REPL MCECTL)

# TEST HARNESS
#FILE (GLOB TESTCPP test/*.cpp)
add_executable(
   Test
   test/Tests.cpp
)
target_link_libraries(Test MCECTL)
target_link_libraries(Test boost_unit_test_framework)
