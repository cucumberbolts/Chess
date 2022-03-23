project(glad)

add_library(${PROJECT_NAME}
    "include/glad/glad.h"
    "include/KHR/khrplatform.h"

    "src/glad.c"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    "include/"
)
