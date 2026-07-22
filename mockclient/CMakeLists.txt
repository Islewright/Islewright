add_executable(islewright-mockclient
    src/main.cpp
)

target_include_directories(islewright-mockclient
    PRIVATE
        ${PROJECT_SOURCE_DIR}/client/src
)

target_link_libraries(islewright-mockclient
    PRIVATE
        Islewright::common
        ws2_32
)