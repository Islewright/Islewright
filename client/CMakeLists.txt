add_executable(islewright-client
    src/main.cpp
)

target_link_libraries(islewright-client
    PRIVATE
        Islewright::common
        SDL3::SDL3
)
