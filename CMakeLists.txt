cmake_minimum_required(VERSION 3.0.0)
project(BLE_DATA_PARSER_DEMO)

# 如果BluetoothConnector.hpp不在当前源目录中，请使用include_directories来添加头文件的路径
# include_directories(path_to_your_header_files)

add_executable(demo test.cc)

# 链接tinyb库和pthread库
target_link_libraries(demo tinyb pthread)

set_target_properties(demo PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
