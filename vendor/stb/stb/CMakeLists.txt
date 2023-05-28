set(PROJECT_NAME stb)

################################################################################
# Source groups
################################################################################
set(Headers
    "stb_image.h"
)
source_group("Headers" FILES ${Headers})

set(ALL_FILES
    ${Headers}
)

add_custom_target(${PROJECT_NAME} SOURCES ${ALL_FILES})

