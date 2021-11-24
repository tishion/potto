###############################
# Ark
project(ark)
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES
    ${PROJECT_NAME}/*.h
    ${PROJECT_NAME}/*.cpp
)
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
    FILES ${${PROJECT_NAME}_SRC_FILES}
)
add_library(${PROJECT_NAME} OBJECT
    ${${PROJECT_NAME}_SRC_FILES}
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER example
)

###############################
# Client
project(client)
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES
    ${PROJECT_NAME}/*.h
    ${PROJECT_NAME}/*.cpp
)
source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
    FILES ${${PROJECT_NAME}_SRC_FILES}
)
add_library(${PROJECT_NAME} OBJECT
    ${${PROJECT_NAME}_SRC_FILES}
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER example
)