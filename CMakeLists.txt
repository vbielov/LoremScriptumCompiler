cmake_minimum_required(VERSION 3.12)

project(lsc C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    set(CMAKE_DEBUG_POSTFIX "_debug")
    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(CMAKE_RELEASE_POSTFIX "_release")
endif()

set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(PROJECT_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include/)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/*.c)

if(NOT BUILD_TESTS)
    add_definitions(-DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING)
    
    find_package(LLD REQUIRED CONFIG)
    find_package(LLVM REQUIRED CONFIG)

    # Get all LLVM's modules and libraries 
    execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND llvm-config --system-libs --libs all OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

    message(STATUS "LLVM CXXFLAGS: ${LLVM_CXXFLAGS}")
    message(STATUS "LLVM LDFLAGS: ${LLVM_LDFLAGS}")
    message(STATUS "LLVM LIBS: ${LLVM_LIBS}")

    include_directories(${LLVM_INCLUDE_DIRS})
    include_directories(${LLD_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})

    if(WIN32)
        set(WINDOWS_RESOURCES ${CMAKE_SOURCE_DIR}/resources/icon.o)
    endif(WIN32)

    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${WINDOWS_RESOURCES}) 

    target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})

    target_compile_options(${PROJECT_NAME} PRIVATE ${LLVM_CXXFLAGS})
    # NOTE: On windows LLD_EXPORTED_TARGETS contains lld library which doesn't come with mingw, and we don't need it.
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ ${LLVM_LIBS} ${LLVM_LDFLAGS} ${LLD_EXPORTED_TARGETS})
    target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})

    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_SEARCH_START_STATIC ON
        LINK_SEARCH_END_STATIC ON
    )
# ---------------------------- TESTING ----------------------------
else()
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}"
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
            ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
            EXCLUDE_FROM_ALL)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif()

    add_subdirectory(test)
    enable_testing()
    add_test(testTrie test/lscTest --gtest_output=xml:report.xml)
endif()
