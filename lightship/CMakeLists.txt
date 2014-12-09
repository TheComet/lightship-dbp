project (lightship)

file (GLOB lightship_HEADERS "include/lightship/*.h")
file (GLOB lightship_SOURCES "src/*.c")

add_executable (lightship ${lightship_HEADERS} ${lightship_SOURCES})
