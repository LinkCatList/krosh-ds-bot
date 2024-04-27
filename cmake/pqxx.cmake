include(FetchContent)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.9.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(libpqxx)

target_link_libraries(${PROJECT_NAME} PRIVATE pqxx)
