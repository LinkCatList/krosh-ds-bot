find_package(OpenSSL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})

include(FetchContent)

target_link_libraries(${PROJECT_NAME} PRIVATE
    dl
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
)

FetchContent_Declare(
    libdpp
    GIT_REPOSITORY https://github.com/brainboxdotcc/DPP.git
    GIT_TAG master
)

add_compile_definitions(DPP_CORO)

FetchContent_MakeAvailable(libdpp)

target_link_libraries(${PROJECT_NAME} PRIVATE dpp)
