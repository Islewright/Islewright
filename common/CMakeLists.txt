add_library(islewright-common STATIC
    src/serializer.cpp
    src/version.cpp
    src/world.cpp
    proto/islewright.proto
)

protobuf_generate(TARGET islewright-common LANGUAGE cpp APPEND_PATH)

add_library(Islewright::common ALIAS islewright-common)

target_compile_features(islewright-common PUBLIC cxx_std_20)

target_include_directories(islewright-common
    PUBLIC
        include
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(islewright-common
    PUBLIC
        EnTT::EnTT
        protobuf::libprotobuf
)

add_executable(islewright-common-tests
    tests/serializer_test.cpp
    tests/world_test.cpp
)
target_link_libraries(islewright-common-tests PRIVATE Islewright::common)
add_test(NAME common-tests COMMAND islewright-common-tests)