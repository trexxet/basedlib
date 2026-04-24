add_library(basedlib INTERFACE)

set_target_properties(basedlib PROPERTIES VERSION 0.1)

target_include_directories(basedlib INTERFACE include/)
target_compile_features(basedlib INTERFACE cxx_std_23)
target_link_libraries(basedlib INTERFACE stdc++exp)

if(MINGW)
	target_compile_options(basedlib INTERFACE -fno-ms-extensions)
endif()
