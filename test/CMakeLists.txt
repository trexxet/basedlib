add_custom_target(basedlib_test_all)

function(add_basedlib_test testName)
	add_executable(${testName} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${testName} basedlib)
	add_dependencies(basedlib_test_all ${testName})
endfunction()

add_basedlib_test(basedlib_test_test testTest.cpp)
add_basedlib_test(basedlib_test_multibyte testMultibyte.cpp)
add_basedlib_test(basedlib_test_logfile testLogFile.cpp)
add_basedlib_test(basedlib_test_ringbuffer testRingBuffer.cpp)
add_basedlib_test(basedlib_test_fsm testFSM.cpp)
add_basedlib_test(basedlib_test_prettyenum testPrettyEnum.cpp)

add_test(NAME basedlib_test_fsm COMMAND basedlib_test_fsm)
add_test(NAME basedlib_test_ringbuffer COMMAND basedlib_test_ringbuffer)
add_test(NAME basedlib_test_test COMMAND basedlib_test_test)
