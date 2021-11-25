project(pt-gen)
file(GLOB_RECURSE ${PROJECT_NAME}_SRC_FILES
    potto-gen/*.h
    potto-gen/*.cpp
)
add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SRC_FILES}
)
set_target_properties(${PROJECT_NAME}
    PROPERTIES
        FOLDER tools
)

if(WINDOWS)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        shlwapi
)
else()
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        dl
)
endif()
