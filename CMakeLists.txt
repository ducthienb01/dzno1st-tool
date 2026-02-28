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

# (Phần include lặp lại - bạn có thể xóa 1 cái nếu muốn sạch sẽ)
target_include_directories(main PRIVATE
    "D:/vcpkg/installed/x64-mingw-dynamic/include"
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

# ==================== TỰ ĐỘNG COPY DLL SAU KHI BUILD (CHO WINDOWS/MINGW) ====================
if(WIN32)
    message(STATUS "Thiết lập tự động copy DLL cho MinGW dynamic...")

    # 1. Copy libcurl.dll từ vcpkg (tự động lấy đường dẫn từ target CURL)
    get_target_property(CURL_DLL_LOCATION CURL::libcurl IMPORTED_LOCATION_RELEASE)
    if(NOT CURL_DLL_LOCATION)
        get_target_property(CURL_DLL_LOCATION CURL::libcurl IMPORTED_LOCATION)
    endif()

    if(CURL_DLL_LOCATION)
        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CURL_DLL_LOCATION}"
                $<TARGET_FILE_DIR:main>
            COMMENT "Copy libcurl.dll từ vcpkg"
        )
    else()
        message(WARNING "Không tìm thấy libcurl.dll từ vcpkg. Copy thủ công nhé!")
    endif()

    # 2. Copy 3 DLL runtime MinGW quan trọng nhất
    # THAY ĐƯỜNG DẪN NÀY THEO MÁY BẠN (kiểm tra bằng explorer)
    # Thường là: C:/mingw64/bin hoặc C:/msys64/mingw64/bin nếu dùng MSYS2
    set(MINGW_BIN "D:/mingw64/bin")  # <-- SỬA ĐƯỜNG DẪN NÀY CHO ĐÚNG!!!

    set(MINGW_DLLS
        libgcc_s_seh-1.dll
        libstdc++-6.dll
        libwinpthread-1.dll
    )

    foreach(DLL ${MINGW_DLLS})
        add_custom_command(TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN}/${DLL}"
                $<TARGET_FILE_DIR:main>
            COMMENT "Copy MinGW runtime: ${DLL}"
        )
    endforeach()

    # 3. Nếu curl dùng OpenSSL/zlib (thường có), thêm copy nếu thiếu (kiểm tra bằng Dependencies tool sau)
    # Uncomment nếu cần:
    # add_custom_command(TARGET main POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #         "D:/vcpkg/installed/x64-mingw-dynamic/bin/libssl-3.dll"   # hoặc libssl-1_1.dll tùy version
    #         $<TARGET_FILE_DIR:main>
    #     COMMENT "Copy libssl.dll (nếu curl dùng OpenSSL)"
    # )
    # add_custom_command(TARGET main POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #         "D:/vcpkg/installed/x64-mingw-dynamic/bin/libcrypto-3.dll"
    #         $<TARGET_FILE_DIR:main>
    #     COMMENT "Copy libcrypto.dll"
    # )
    # add_custom_command(TARGET main POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #         "D:/vcpkg/installed/x64-mingw-dynamic/bin/zlib1.dll"
    #         $<TARGET_FILE_DIR:main>
    #     COMMENT "Copy zlib1.dll (nếu cần)"
    # )

    message(STATUS "Auto-copy DLL setup hoàn tất. Build Release để deploy.")
endif()