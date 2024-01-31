include_directories(../graph ../tools)

add_library(kcliqueEnumerator_kclist kclist.h enumerator.h kclist.cpp)
target_link_libraries(kcliqueEnumerator_kclist types Graph)

add_library(Enumerator_bufferKclist bufferKclist.h enumerator.h bufferKclist.cpp)
target_link_libraries(Enumerator_bufferKclist types Graph)

add_library(SCT sct.h enumerator.h sct.cpp sctSave.cpp sctSampling.cpp sctShrink.cpp)
target_link_libraries(SCT types Graph linearSet comb)
