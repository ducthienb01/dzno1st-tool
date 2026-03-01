cmake_minimum_required(VERSION 3.15)
project(SeleniumAuto LANGUAGES CXX)
if(POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tắt warning Clang nếu cần (giữ nguyên)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wno-return-type -Wno-missing-return -Wno-multichar
        -Wno-char-constant-too-long -Wno-constant-conversion
        -Wno-unused-value -Wno-multichar-constant
        -Wno-duplicate-decl-specifier
    )
endif()

# CURL: Dùng CONFIG REQUIRED + vcpkg sẽ tự tìm (nếu toolchain set)
find_package(CURL CONFIG REQUIRED)

if(NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found. Install via vcpkg: vcpkg install curl:x64-mingw-dynamic")
endif()

add_executable(main
    src/main.cpp
    src/golike.cpp
    src/instagram.cpp
    src/Loadjob.cpp
    src/UI.cpp
    src/TDS.cpp
)

# Include: Chỉ giữ project-specific (vcpkg tự handle CURL include)
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/header
    ${CMAKE_CURRENT_SOURCE_DIR}/webdriverxx/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(main PRIVATE
    -g -O0 -fexceptions -fno-omit-frame-pointer
    # Uncomment nếu muốn warning: -Wall -Wextra
)

target_link_libraries(main PRIVATE CURL::libcurl)

if(WIN32)
    target_link_libraries(main PRIVATE bcrypt ws2_32)  # Thêm ws2_32 cho socket nếu curl cần
endif()

# AUTO COPY DLL (cải thiện detect MinGW bin)
if(WIN32)
    message(STATUS "Auto-copy DLL setup for MinGW...")

    # Copy libcurl.dll từ vcpkg
    get_target_property(CURL_DLL CURL::libcurl IMPORTED_LOCATION_RELEASE)
    if(NOT CURL_DLL)
        get_target_property(CURL_DLL CURL::libcurl IMPORTED_LOCATION)
    endif()
    if(CURL_DLL)
        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CURL_DLL}" $<TARGET_FILE_DIR:main>
            COMMENT "Copy libcurl.dll"
        )
    else()
        message(WARNING "Không tìm CURL DLL. Copy thủ công nếu exe lỗi.")
    endif()

    # Detect MinGW bin từ compiler path (tự động, không hardcode)
    get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)

    set(MINGW_DLLS libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)

    foreach(DLL ${MINGW_DLLS})
        set(DLL_PATH "${MINGW_BIN}/${DLL}")
        if(EXISTS "${DLL_PATH}")
            add_custom_command(TARGET main POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_PATH}" $<TARGET_FILE_DIR:main>
                COMMENT "Copy ${DLL}"
            )
        else()
            message(WARNING "Không tìm ${DLL} tại ${DLL_PATH}. Kiểm tra MinGW.")
        endif()
    endforeach()

    # Nếu thiếu libssl/libcrypto/zlib → install openssl qua vcpkg và uncomment copy
    # Ví dụ:
    # add_custom_command(TARGET main POST_BUILD COMMAND ... "libssl-3.dll" ...)
endif()

message(STATUS "CMake ready. Build Release cho exe portable.")