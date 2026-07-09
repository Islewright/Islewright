add_executable(islewright-server
    src/main.cpp
    src/server.hpp
    src/clienthandler.hpp
    src/define.hpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)
