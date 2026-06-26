add_library(islewright-common STATIC
    src/version.cpp
)

add_library(Islewright::common ALIAS islewright-common)

target_compile_features(islewright-common PUBLIC cxx_std_20)

target_include_directories(islewright-common
    PUBLIC
        include
)
