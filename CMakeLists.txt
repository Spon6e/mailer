cmake_minimum_required(VERSION 3.21)

project(auto_mailer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(auto_mailer
    src/main.cpp
    src/db/mail_repository.cpp
    src/mail/dry_run_mail_sender.cpp
    src/mail/mail_worker.cpp
)

set(SQLITE_ROOT
    "C:/Qt/Tools/mingw1310_64/opt")

add_library(SQLite3::SQLite3 STATIC IMPORTED)

set_target_properties(SQLite3::SQLite3 PROPERTIES
    IMPORTED_LOCATION
        "${SQLITE_ROOT}/lib/libsqlite3.a"

    INTERFACE_INCLUDE_DIRECTORIES
        "${SQLITE_ROOT}/include"
)

target_link_libraries(auto_mailer
    PRIVATE SQLite3::SQLite3
)


target_include_directories(auto_mailer PRIVATE include)

if (MSVC)
    target_compile_options(auto_mailer PRIVATE /W4 /permissive-)
else()
    target_compile_options(auto_mailer PRIVATE -Wall -Wextra -Wpedantic)
endif()
