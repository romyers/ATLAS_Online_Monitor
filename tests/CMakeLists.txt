# Get Catch2

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.6.0
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)


set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_executable(testSettingsManager SettingsManager.test.cpp ${SRC_DIR}/SettingsManager.cpp)
target_link_libraries(testSettingsManager PRIVATE Catch2::Catch2WithMain)
target_include_directories(testSettingsManager PRIVATE ${SRC_DIR})
add_test(NAME testSettingsManager COMMAND testSettingsManager)
catch_discover_tests(testSettingsManager)