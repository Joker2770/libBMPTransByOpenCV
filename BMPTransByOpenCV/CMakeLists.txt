cmake_minimum_required(VERSION 2.8)

project(BMPTransByOpenCV)

#set(CMAKE_CXX_FLAGS   "-std=c++11")

#优先从".:./libopencv_2.4.13.6:./drv-lib/libopencv_2.4.13.6"搜索依赖库
set(CMAKE_CXX_FLAGS "-g -rdynamic -fPIC -Wl,-rpath=.:./libopencv_2.4.13.6:./drv-lib/libopencv_2.4.13.6")

find_package(OpenCV 2 REQUIRED opencv_imgproc opencv_highgui)

include_directories(${OpenCV_INCLUDE_DIRS})
# directory of opencv library
link_directories(${OpenCV_LIBRARY_DIRS})

aux_source_directory(. DIR_LIB_SRCS)

add_library(${PROJECT_NAME} SHARED ${DIR_LIB_SRCS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../solib) 
