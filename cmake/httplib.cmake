include(FetchContent)

FetchContent_Declare(
    httplib SYSTEM
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.14.1
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(httplib)

target_link_libraries(${PROJECT_NAME} PRIVATE httplib)