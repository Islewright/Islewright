add_executable(islewright-server
    src/main.cpp
    src/chunkresponse.cpp
    src/chunkresponse.hpp
    src/clientinfo.hpp
    src/clientconnector.hpp
    src/clientsession.hpp
    src/gameinstance.hpp
    src/gameloop.hpp
)

target_link_libraries(islewright-server
    PRIVATE
        Islewright::common
        ws2_32
)

add_executable(islewright-server-tests
    src/chunkresponse.cpp
    tests/gameinstance_test.cpp
)

target_include_directories(islewright-server-tests PRIVATE src)
target_link_libraries(islewright-server-tests PRIVATE Islewright::common)
add_test(NAME server-tests COMMAND islewright-server-tests)
