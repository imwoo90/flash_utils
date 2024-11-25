cmake_minimum_required(VERSION 3.23.0)
project(flash_utils VERSION 0.1.0 LANGUAGES C CXX)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 라이브러리 소스 파일 설정
add_library(flash_utils)
add_subdirectory(src)

# include 디렉토리를 공개 헤더로 설정
target_include_directories(flash_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# GoogleTest 다운로드 (FetchContent 사용)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# GTest를 빌드에 추가
FetchContent_MakeAvailable(googletest)

# CTest 연동
include(CTest)
enable_testing()

# 테스트 디렉토리 추가
add_subdirectory(test)

