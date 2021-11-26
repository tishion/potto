if (NOT POTTO_BUILD_STATIC)
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.21")
        install(IMPORTED_RUNTIME_ARTIFACTS potto
            CONFIGURATIONS Release 
                DESTINATION example
        )
        install(IMPORTED_RUNTIME_ARTIFACTS potto
            CONFIGURATIONS Debug 
                DESTINATION debug/example
        )
    else ()
        install(FILES $<TARGET_FILE:potto>
            CONFIGURATIONS Release 
                DESTINATION example
        )
        install(FILES $<TARGET_FILE:potto>
            CONFIGURATIONS Debug 
                DESTINATION debug/example
        )
    endif()
endif()

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

add_library(${PROJECT_NAME} SHARED
    ${${PROJECT_NAME}_SRC_FILES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER example
)

target_link_libraries(${PROJECT_NAME}
    potto
)

#  call pt-gen to generate the CLSID header file for this module
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "$<TARGET_FILE:pt-gen>" -classid 
        "$<TARGET_FILE:${PROJECT_NAME}>" 
        "${CMAKE_CURRENT_SOURCE_DIR}/ark/include"
        "${PROJECT_NAME}_CLSID.h"

    DEPENDS
        "$<TARGET_FILE:pt-gen>"
        "$<TARGET_FILE:${PROJECT_NAME}>"

    VERBATIM
)

install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Release
        DESTINATION example
)

install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Debug
        DESTINATION debug/example
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

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SRC_FILES}
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER example
)

target_link_libraries(${PROJECT_NAME}
    potto
)

add_dependencies(${PROJECT_NAME}
    ark
)

if (NOT POTTO_BUILD_STATIC)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:potto>" 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

    DEPENDS
        "$<TARGET_FILE:potto>"

    VERBATIM
)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "$<TARGET_FILE:pt-gen>" -modulelib 
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/modulelib.xml"

    DEPENDS
        "$<TARGET_FILE:pt-gen>"
        "$<TARGET_FILE:ark>"

    VERBATIM
)

install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Release
        DESTINATION example
)

install(TARGETS ${PROJECT_NAME}
    CONFIGURATIONS Debug
        DESTINATION debug/example
)

install(FILES $<TARGET_FILE_DIR:${PROJECT_NAME}>/modulelib.xml
    CONFIGURATIONS Release
        DESTINATION example
)

install(DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>/modulelib.xml
    CONFIGURATIONS Debug
        DESTINATION debug/example
)