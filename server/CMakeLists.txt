add_executable(islewright-server
    src/main.cpp
    src/clientinfo.hpp
    src/clientconnector.hpp
    src/clientsession.hpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)
