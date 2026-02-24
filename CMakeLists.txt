cmake_minimum_required(VERSION 3.15)
project(SeleniumAuto LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tắt warning cực mạnh cho Clang (toàn cục)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wno-return-type
        -Wno-missing-return
        -Wno-multichar
        -Wno-char-constant-too-long
        -Wno-constant-conversion
        -Wno-unused-value
        -Wno-multichar-constant
    )
endif()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/header
    ${CMAKE_CURRENT_SOURCE_DIR}/webdriverxx/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(CURL REQUIRED)
if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Install: brew install curl")
endif()

add_executable(main
    src/main.cpp
    src/golike.cpp
    src/instagram.cpp
    src/Loadjob.cpp
    src/UI.cpp
    src/TDS.cpp
)

# Thêm flags debug + exception handling (để catch exception không crash)
target_compile_options(main PRIVATE
    -g                       # Thêm debug symbols để backtrace rõ ràng
    -O0                      # Tắt tối ưu hóa để exception unwind dễ dàng
    -fexceptions             # Bật đầy đủ hỗ trợ exception (rất quan trọng trên macOS)
    -fno-omit-frame-pointer  # Giữ frame pointer để stack trace chi tiết trong lldb
)

target_link_libraries(main PRIVATE CURL::libcurl)
if(WIN32)
    target_link_libraries(main PRIVATE bcrypt)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(main PRIVATE bcrypt)
endif()
target_compile_features(main PRIVATE cxx_std_17)

# Lặp lại flag tắt warning ở target để chắc chắn
# Warning flags chỉ áp dụng cho Clang (macOS native), không áp dụng cho MinGW
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(main PRIVATE
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

# Nếu muốn tắt gần như hết warning (tạm thời, không khuyến khích lâu dài)
# add_compile_options(-w)