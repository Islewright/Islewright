add_executable(islewright-server
    src/main.cpp
    src/IslewrightServer.hpp
    src/clientInfo.hpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)
