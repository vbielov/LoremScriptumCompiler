file(GLOB SRCS *.cpp)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_definitions(-DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING)

find_package(LLD REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)


# Get LLVM compile and link flags from llvm-config
execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs --libs all OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Print found values (for debugging)
message(STATUS "LLVM CXXFLAGS: ${LLVM_CXXFLAGS}")
message(STATUS "LLVM LDFLAGS: ${LLVM_LDFLAGS}")
message(STATUS "LLVM LIBS: ${LLVM_LIBS}")


include_directories(${LLD_INCLUDE_DIRS})


include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*[/\\]main\\.cpp$")
add_executable(lscTest ${SRCS} ${PROJECT_SOURCES})
target_include_directories(lscTest PRIVATE ${INCLUDE_DIR})

target_link_libraries(lscTest PRIVATE gtest gmock -static-libgcc -static-libstdc++ ${LLVM_LIBS} ${LLVM_LDFLAGS} ${LLD_EXPORTED_TARGETS})
set_target_properties(lscTest PROPERTIES
    LINK_SEARCH_START_STATIC ON
    LINK_SEARCH_END_STATIC ON
)

include_directories("..")

install(TARGETS lscTest DESTINATION bin)


