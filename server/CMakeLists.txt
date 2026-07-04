add_executable(islewright-server
    src/main.cpp
    src/network/Define.hpp
    src/network/ClientInfo.hpp
    src/network/IOCPServer.hpp
    src/network/IslewrighterServer.hpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)
