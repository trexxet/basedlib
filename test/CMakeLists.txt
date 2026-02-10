function(add_basedlib_test test_name)
	add_executable(${test_name} EXCLUDE_FROM_ALL ${ARGN})
	target_link_libraries(${test_name} basedlib)
endfunction()

add_basedlib_test(basedlib_test_multibyte testMultibyte.cpp)
add_basedlib_test(basedlib_test_logfile testLogFile.cpp)
add_basedlib_test(basedlib_test_ringbuffer testRingBuffer.cpp)
