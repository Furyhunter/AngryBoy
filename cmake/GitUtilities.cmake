# Git utilities

if(__GitUtilities)
	return()
endif()

set(__GitUtilities ON)

find_program(GIT_EXECUTABLE git)

function(git_describe variable path)
	execute_process(COMMAND "${GIT_EXECUTABLE}" "describe" "--dirty" "--always"
		WORKING_DIRECTORY "${path}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE output
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	#if(NOT result EQUAL 0)
	#	set(${variable} "GITERROR-${result}-NOTFOUND" CACHE STRING "revision" FORCE)
	#endif()

	set(${variable} "${output}" PARENT_SCOPE)
endfunction()

function(git_current_branch variable path)
	execute_process(COMMAND "${GIT_EXECUTABLE}" "symbolic-ref" "--short" "HEAD"
		WORKING_DIRECTORY "${path}"
		RESULT_VARIABLE result
		OUTPUT_VARIABLE output
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	set(${variable} "${output}" PARENT_SCOPE)
endfunction()
