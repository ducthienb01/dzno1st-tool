cmake_minimum_required(VERSION 3.15)
project(SeleniumAuto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tắt warning cho Clang (macOS/Linux clang) - phần này ok ở đầu
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wno-return-type
        -Wno-missing-return
        -Wno-multichar
        -Wno-char-constant-too-long
        -Wno-constant-conversion
        -Wno-unused-value
        -Wno-multichar-constant
        -Wno-duplicate-decl-specifier
    )
endif()

# Tìm CURL (ưu tiên CONFIG cho vcpkg)
find_package(CURL CONFIG QUIET)
if(NOT CURL_FOUND)
    find_package(CURL REQUIRED)
endif()

if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Kiểm tra toolchain vcpkg và vcpkg.json.")
endif()

# Định nghĩa target trước (quan trọng!)
add_executable(main
    src/main.cpp
    src/golike.cpp
    src/instagram.cpp
    src/Loadjob.cpp
    src/UI.cpp
    src/TDS.cpp
)

# Bây giờ mới cấu hình target (include, compile options, link)
target_include_directories(main PRIVATE
    "D:/vcpkg/installed/x64-mingw-dynamic/include"
    ${CMAKE_CURRENT_SOURCE_DIR}/header
    ${CMAKE_CURRENT_SOURCE_DIR}/webdriverxx/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    BEFORE
)
target_include_directories(main PRIVATE
    "D:/vcpkg/installed/x64-mingw-dynamic/include"   # hoặc đường dẫn vcpkg bạn đang dùng
    # giữ nguyên các include khác: header, webdriverxx/include, v.v.
)
target_compile_options(main PRIVATE
    -g
    -O0
    -fexceptions
    -fno-omit-frame-pointer
)

target_link_libraries(main PRIVATE CURL::libcurl)

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(main PRIVATE bcrypt)
endif()

# Nếu muốn tắt hết warning tạm thời (chỉ dùng tạm)
# target_compile_options(main PRIVATE -w)