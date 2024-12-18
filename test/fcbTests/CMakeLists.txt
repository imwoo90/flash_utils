# 테스트용 CMake 파일
add_executable(fcbTests fcbTests.cpp)

# GTest 및 YourLibrary와 연결
target_link_libraries(fcbTests gtest_main flash_utils)

# 테스트 실행 추가 (CMake 3.10 이상 필요)
include(GoogleTest)
gtest_discover_tests(fcbTests)