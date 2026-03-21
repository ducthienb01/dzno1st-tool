cmake_minimum_required(VERSION 3.15...3.28)

# === PHẢI Ở ĐẦU ===
set(VCPKG_TARGET_TRIPLET "x64-mingw-static" CACHE STRING "Vcpkg target triplet")

# THÊM STATIC LINKAGE
set(VCPKG_LIBRARY_LINKAGE static CACHE STRING "")
set(VCPKG_CRT_LINKAGE static CACHE STRING "")

# TẮT AUTO INSTALL
set(VCPKG_MANIFEST_MODE OFF CACHE BOOL "Disable manifest mode")
set(VCPKG_MANIFEST_INSTALL OFF CACHE BOOL "Disable auto install")

project(SeleniumAuto LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(POLICY CMP0003)
    cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wno-return-type -Wno-missing-return -Wno-multichar
        -Wno-char-constant-too-long -Wno-constant-conversion
        -Wno-unused-value -Wno-multichar-constant
        -Wno-duplicate-decl-specifier
    )
endif()

find_package(CURL CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
# === FALLBACK CỨNG (để chắc chắn include path) ===
if(NOT CURL_INCLUDE_DIRS OR CURL_INCLUDE_DIRS STREQUAL "")
    set(CURL_INCLUDE_DIRS "D:/vcpkg/installed/x64-mingw-static/include")
    message(WARNING "Fallback CURL_INCLUDE_DIRS to: ${CURL_INCLUDE_DIRS}")
endif()
if(NOT PYBIND11_INCLUDE_DIRS OR PYBIND11_INCLUDE_DIRS STREQUAL "")
    set(PYBIND11_INCLUDE_DIRS "C:/Users/TT/AppData/Local/Python/pythoncore-3.14-64/Lib/site-packages/pybind11/include")
    message(STATUS "Using PYBIND11_INCLUDE_DIRS: ${PYBIND11_INCLUDE_DIRS}")
endif()
if(NOT CURL_FOUND)
    message(FATAL_ERROR "CURL not found!")
endif()

add_executable(main
    src/main.cpp
    src/golike.cpp
    src/instagram.cpp
    src/Loadjob.cpp
    src/UI.cpp
    src/TDS.cpp
    src/adb.cpp
    # src/Open_Cv.cpp
    src/golike_new.cpp
)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/header
    ${CMAKE_CURRENT_SOURCE_DIR}/webdriverxx/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    
    # Thêm fallback cứng
    ${CURL_INCLUDE_DIRS}
    "D:/vcpkg/installed/x64-mingw-static/include"
    ${PYBIND11_INCLUDE_DIRS}
)

target_compile_options(main PRIVATE
    -g -O0 -fexceptions -fno-omit-frame-pointer
)

target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main PRIVATE
    CURL::libcurl
    # ${OpenCV_LIBS}
    protobuf::libprotobuf
    Python3::Python
)

# THÊM STATIC LINK OPTIONS (để không cần DLL MinGW)
target_link_options(main PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
)

if(WIN32)
    target_link_libraries(main PRIVATE bcrypt ws2_32)
endif()
if(WIN32)
    get_filename_component(PYTHON_DLL_DIR "${Python3_RUNTIME_LIBRARY_DIRS}" ABSOLUTE)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Python3_RUNTIME_LIBRARY}"
        $<TARGET_FILE_DIR:main>
        COMMENT "Copy Python runtime DLL"
    )

endif()
# Copy DLL (giữ nguyên)
if(WIN32)
    message(STATUS "Auto-copy DLL setup for MinGW...")

    if(TARGET CURL::libcurl)
        get_target_property(CURL_DLL CURL::libcurl IMPORTED_LOCATION_RELEASE)
        if(NOT CURL_DLL)
            get_target_property(CURL_DLL CURL::libcurl IMPORTED_LOCATION)
        endif()
        if(CURL_DLL)
            add_custom_command(TARGET main POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CURL_DLL}" $<TARGET_FILE_DIR:main>
                COMMENT "Copy libcurl.dll"
            )
        endif()
    endif()

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
            message(WARNING "Không tìm ${DLL} tại ${DLL_PATH}")
        endif()
    endforeach()
endif()

message(STATUS "CMake ready. Build Release cho exe portable.")