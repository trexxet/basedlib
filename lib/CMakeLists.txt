add_library(basedlib STATIC)

target_link_options(basedlib PRIVATE
	-Wl,--export-all-symbols
)
set_target_properties(basedlib PROPERTIES
	VERSION 0.1
	DEBUG_POSTFIX "dbg"
)
target_compile_definitions(basedlib PRIVATE
	$<$<CONFIG:Debug>:BASEDLIB_DEBUG>
)

target_sources(basedlib PRIVATE
	$<$<CONFIG:Debug>:src/Debug.cpp>
)
target_include_directories(basedlib PUBLIC include/)
