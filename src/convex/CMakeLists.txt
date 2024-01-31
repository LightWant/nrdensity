include_directories(../graph ../tools ../kcliqueEnumerator)

add_library(kclistpp kclistpp.h kclistpp.cpp kclistppkclflow.cpp)
target_link_libraries(kclistpp SCT Enumerator_bufferKclist kcliqueEnumerator_kclist  types Graph comb)

add_library(minrf minrf.h minrf.cpp cliqueFinderSCT.hpp cliqueFinderKCL.hpp)
target_link_libraries(minrf types Graph kcliqueEnumerator_kclist)