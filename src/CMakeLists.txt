file(GLOB SRCS *.cpp *.c *.rc)
file(GLOB HDRS *.hpp *.h)

configure_file("config.h.in" "config.h")

add_executable(AngryBoy ${SRCS} ${HDRS} "${CMAKE_CURRENT_BINARY_DIR}/config.h" "config.h.in")

install(TARGETS AngryBoy DESTINATION .)