project(main)
find_package(OpenCV REQUIRED)
add_executable(main main)
target_link_libraries(main ${OpenCV_LIBS})