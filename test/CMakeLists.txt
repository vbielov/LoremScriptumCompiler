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

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*[/\\]main\\.cpp$")
add_executable(lscTest ${SRCS} ${PROJECT_SOURCES})
target_include_directories(lscTest PRIVATE ${INCLUDE_DIR})

target_link_libraries(lscTest
    gtest
    gmock
)

include_directories("..")

install(TARGETS lscTest DESTINATION bin)


