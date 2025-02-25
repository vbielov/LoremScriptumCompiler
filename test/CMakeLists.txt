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

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM: ${LLVM_DIR}")
message(STATUS "Using LLVM version: ${LLVM_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS core irreader support)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*[/\\]main\\.cpp$")
add_executable(lscTest ${SRCS} ${PROJECT_SOURCES})
target_include_directories(lscTest PRIVATE ${INCLUDE_DIR})

target_link_libraries(lscTest PRIVATE gtest gmock -static-libgcc -static-libstdc++ ${LLVM_LIBS})

include_directories("..")

install(TARGETS lscTest DESTINATION bin)


