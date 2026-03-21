# toolchain-mingw-windows.cmake

# Tên hệ điều hành mục tiêu
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Fix Apple Silicon host tự thêm -arch arm64 vào compiler MinGW
if(APPLE AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(CMAKE_OSX_ARCHITECTURES "")  # Xóa architectures mặc định của host
endif()

# Ép target là x86_64, loại bỏ flag -arch arm64 (nếu có)
set(CMAKE_C_FLAGS_INIT   "-m64")
set(CMAKE_CXX_FLAGS_INIT "-m64")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-m64")

# Loại bỏ -arch arm64 nếu CMake thêm vào bất kỳ đâu
string(REPLACE "-arch arm64" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-arch arm64" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-arch arm64" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

# Chỉ định compiler từ mingw-w64 (brew install mingw-w64)
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

# Quan trọng cho cross-compile
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Bật static linking (giảm DLL dependencies, khuyến khích cho Windows)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")  # Ưu tiên static libs
add_link_options(-static -static-libgcc -static-libstdc++)

# Chainload vcpkg toolchain (để tìm CURL, zlib, v.v. từ vcpkg)
set(VCPKG_TARGET_TRIPLET x64-mingw-static)
include(~/vcpkg/scripts/buildsystems/vcpkg.cmake)   # Đảm bảo path ~/vcpkg đúng (kiểm tra bằng ls ~/vcpkg)