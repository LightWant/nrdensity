cmake_minimum_required(VERSION 3.6)

project(densestSubgraph)

SET(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")
SET(EXECUTABLE_OUTPUT_PATH "bin/")

set(exName1 "kclisttest")
set(exName2 "betalocalDensity")
set(exName3 "testBoostMincut")
set(exName4 "adsRunner")
set(exName5 "notDegenerate")
set(exName6 "testLocalCondunctance")
set(exName7 "flowseedsRunner")
set(exName8 "exactLocalDensity")
set(exName9 "seedsGenerator")

add_subdirectory(src/graph)
add_subdirectory(src/kcliqueEnumerator)
add_subdirectory(src/tools)
add_subdirectory(src/convex)
add_subdirectory(src/sampler)
add_subdirectory(src/lads)
add_subdirectory(src/flowseeds)

add_executable(${exName1} src/runner/kclistppRunner.cpp)
add_executable(${exName2} src/runner/localDensity.cpp)
add_executable(${exName3} src/runner/testBoostMincut.cpp)
add_executable(${exName4} src/runner/adsRunner.cpp)
add_executable(${exName5} src/runner/notDegenerate.cpp)
add_executable(${exName6} src/runner/testLocalCondunctance.cpp)
add_executable(${exName7} src/runner/flowseedsRunner.cpp)
add_executable(${exName8} src/runner/exactLocalDensity.cpp)
add_executable(${exName9} src/runner/seedsGenerator.cpp)

target_link_libraries(${exName1} getArgs)
target_link_libraries(${exName1} Graph)
target_link_libraries(${exName1} kcliqueEnumerator_kclist)
target_link_libraries(${exName1} Enumerator_bufferKclist)
target_link_libraries(${exName1} kclistpp)

target_link_libraries(${exName2} getArgs)
target_link_libraries(${exName2} fastIO)
target_link_libraries(${exName2} types)

target_link_libraries(${exName3} HLPP)
target_link_libraries(${exName3} types)

target_link_libraries(${exName4} getArgs)
target_link_libraries(${exName4} Graph)
target_link_libraries(${exName4} LADSHLPP)
# target_link_libraries(${exName4} LADSDinic)


target_link_libraries(${exName5} getArgs)
target_link_libraries(${exName5} Graph)
target_link_libraries(${exName5} types)

target_link_libraries(${exName6} getArgs)
target_link_libraries(${exName6} Graph)
target_link_libraries(${exName6} types)

target_link_libraries(${exName7} getArgs)
target_link_libraries(${exName7} Graph)
target_link_libraries(${exName7} types)
target_link_libraries(${exName7} flowseeds)

target_link_libraries(${exName8} getArgs)
target_link_libraries(${exName8} fastIO)
target_link_libraries(${exName8} types)
target_link_libraries(${exName8} HLPP)

target_link_libraries(${exName9} getArgs)
target_link_libraries(${exName9} fastIO)
target_link_libraries(${exName9} types)
target_link_libraries(${exName9} Graph)