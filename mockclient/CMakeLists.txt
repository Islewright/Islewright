add_executable(islewright-mockclient
    src/main.cpp
    src/command.hpp
    src/networkmanager.hpp
    src/tcpconnector.hpp
)


target_link_libraries(islewright-mockclient
    PRIVATE
        Islewright::common
        ws2_32
)

add_executable(islewright-mockclient-tests
    tests/command_test.cpp
)
target_include_directories(islewright-mockclient-tests PRIVATE src)
target_link_libraries(islewright-mockclient-tests PRIVATE Islewright::common)
add_test(NAME mockclient-tests COMMAND islewright-mockclient-tests)
