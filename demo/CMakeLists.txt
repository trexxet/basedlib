add_custom_target(basedlib_demo_all)

function(add_basedlib_demo demoName)
	add_executable(${demoName} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${demoName} basedlib)
	add_dependencies(basedlib_demo_all ${demoName})
endfunction()

add_basedlib_demo(basedlib_demo_fsm demoFSM.cpp)
add_basedlib_demo(basedlib_demo_multibyte demoMultibyte.cpp)
add_basedlib_demo(basedlib_demo_prettyenum demoPrettyEnum.cpp)
add_basedlib_demo(basedlib_demo_test demoTest.cpp)
