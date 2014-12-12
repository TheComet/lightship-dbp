project (util)

file (GLOB lightship_util_HEADERS "include/util/*.h")
file (GLOB lightship_util_SOURCES "src/*.c")

add_library (lightship_util STATIC
    ${lightship_util_HEADERS}
    ${lightship_util_SOURCES}
)
