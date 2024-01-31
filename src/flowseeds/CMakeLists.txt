include_directories(../graph ../tools)

add_library(flowseeds flowseedsWithA.h flowseedsWithA.cpp)
target_link_libraries(flowseeds types Graph HLPP)

