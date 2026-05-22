cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Test)

# Включаем родительскую директорию для доступа к исходникам
set(TEST_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}
)

# ============================================================
# Тест для QtFolderScanner
# ============================================================
add_executable(tst_QtFolderScanner
    tst_QtFolderScanner.cpp
    ${CMAKE_SOURCE_DIR}/QtFolderScanner.cpp
)

target_include_directories(tst_QtFolderScanner PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(tst_QtFolderScanner
    PRIVATE Qt6::Core Qt6::Quick Qt6::Test
)
add_test(NAME tst_QtFolderScanner COMMAND tst_QtFolderScanner)

# ============================================================
# Тест для DeploymentManager
# ============================================================
add_executable(tst_DeploymentManager
    tst_DeploymentManager.cpp
    ${CMAKE_SOURCE_DIR}/DeploymentManager.cpp
)

target_include_directories(tst_DeploymentManager PRIVATE ${TEST_INCLUDE_DIRS})
target_link_libraries(tst_DeploymentManager
    PRIVATE Qt6::Core Qt6::Quick Qt6::Test
)
add_test(NAME tst_DeploymentManager COMMAND tst_DeploymentManager)
