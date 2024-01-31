include_directories(../graph ../tools)

add_library(LADS lads.h lads.cpp)
target_link_libraries(LADS types Graph MaxFlowDouble)

add_library(LADSDinic ladsDinic.h ladsDinic.cpp)
target_link_libraries(LADSDinic types Graph dinic)

add_library(LADSHLPP ladsHLPP.h ladsHLPP.cpp)
target_link_libraries(LADSHLPP types Graph HLPP)
