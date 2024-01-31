include_directories(../graph ../tools)

add_library(ccpath ccpath.h ccpath2.cpp)
target_link_libraries(ccpath types Graph comb)

add_library(priCcpath priCcpath.h priCcpath.cpp)
target_link_libraries(priCcpath types Graph)