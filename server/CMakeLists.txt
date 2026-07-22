add_executable(islewright-server
    src/main.cpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)
