add_library(islewright-common STATIC
    src/version.cpp
    src/world.cpp
)

add_library(Islewright::common ALIAS islewright-common)

target_compile_features(islewright-common PUBLIC cxx_std_20)

target_include_directories(islewright-common
    PUBLIC
        include
)

target_link_libraries(islewright-common
    PUBLIC
        EnTT::EnTT
)

add_executable(islewright-common-tests tests/world_test.cpp)
target_link_libraries(islewright-common-tests PRIVATE Islewright::common)
add_test(NAME common-tests COMMAND islewright-common-tests)
