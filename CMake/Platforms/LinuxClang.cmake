message(STATUS "Configuring for platform Linux/Clang.")

set (DELIBERATION_BUILD_TARGET_OS 		  linux64)
set (DELIBERATION_BUILD_COMPILER_PLATFORM clang++-libstdc++)

# Enable C++11 support

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#include_directories("/usr/include/c++/v1/")

set(LINUX_COMPILE_DEFS
	LINUX	                  # Linux system
        PIC		          # Position-independent code
	_REENTRANT                # Reentrant code
)
set(DEFAULT_COMPILE_DEFS_DEBUG
    ${LINUX_COMPILE_DEFS}
    _DEBUG	                  # Debug build
)
set(DEFAULT_COMPILE_DEFS_RELEASE
    ${LINUX_COMPILE_DEFS}
    NDEBUG	                  # Release build
)

if (OPTION_ERRORS_AS_EXCEPTION)
	set(EXCEPTION_FLAG "-fexceptions")
else()
	set(EXCEPTION_FLAG "-fno-exceptions")
endif()

set(LINUX_COMPILE_FLAGS
      
      ${EXCEPTION_FLAG}
      -pthread      # -> use pthread library
    # -no-rtti      # -> disable c++ rtti
      -pipe         # -> use pipes
      -pedantic     # ->
      -Wall         # -> 
      -Wextra       # -> 
      -Werror       # ->
    # -pedantic     # -> yeah, be really pedantic
	
      -Wfloat-equal 
      -Wcast-qual 
      -Wcast-align 
      -Wconversion 
      -Wreturn-type 

      -Wno-error=float-equal 
      -Wno-error=conversion 
      -Wno-error=switch
      -Wno-error=comment
      -Wno-error=strict-aliasing
      
      -Wno-mismatched-tags 
      -Wno-nested-anon-types 
      -Wno-unsequenced 
      -Wno-sign-conversion 
      -Wno-unused-function 
      -Wno-missing-braces 
      -Wno-error=shorten-64-to-32

      -fPIC         # -> use position independent code
      -fvisibility=hidden # ->
    # -Werror=return-type -> missing returns in functions and methods are handled as errors which stops the compilation
    # -Wshadow      # -> e.g. when a parameter is named like a member, too many warnings, disabled for now
)

set(DEFAULT_COMPILE_FLAGS
    ${LINUX_COMPILE_FLAGS}
    $<$<CONFIG:Debug>:   
    >
    $<$<CONFIG:Release>: 
    >
)

set(LINUX_LINKER_FLAGS "-pthread")

set(DEFAULT_LINKER_FLAGS_RELEASE ${LINUX_LINKER_FLAGS})
set(DEFAULT_LINKER_FLAGS_DEBUG ${LINUX_LINKER_FLAGS})
set(DEFAULT_LINKER_FLAGS ${LINUX_LINKER_FLAGS})

# Add platform specific libraries for linking
set(EXTRA_LIBS "")
