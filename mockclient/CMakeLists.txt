add_executable(islewright-mockclient
    src/main.cpp
    src/TcpClient.hpp
)

target_link_libraries(islewright-mockclient
    PRIVATE
        Islewright::common
        ws2_32
)
