add_executable(islewright-client
    src/input.cpp
    src/main.cpp
 )

target_link_libraries(islewright-client
    PRIVATE
        Islewright::common
        SDL3::SDL3
        ws2_32
)
