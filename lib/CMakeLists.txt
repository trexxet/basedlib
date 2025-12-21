add_library(basedlib STATIC)

target_link_options(basedlib PRIVATE
	-Wl,--export-all-symbols
)
set_target_properties(basedlib PROPERTIES
	VERSION 0.1
	DEBUG_POSTFIX "dbg"
)

set(BASED_SOURCES
	FileDebug.cpp
)
list(TRANSFORM BASED_SOURCES PREPEND src/)

target_sources(basedlib PRIVATE ${BASED_SOURCES})
target_include_directories(basedlib PUBLIC include/)
