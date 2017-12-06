cmake_minimum_required(VERSION 3.0)
project(imagesegmentation VERSION 1.0.0)
set(clandmark_VERSION_MAJOR 1)
set(clandmark_VERSION_MINOR 0)
set(clandmark_VERSION ${clandmark_VERSION_MAJOR}.${clandmark_VERSION_MINOR})
set(CMAKE_BUILD_TYPE Release)

set(TARGET imagesegmentation)
add_library(${TARGET} SHARED ImageSegmentation.cpp Line.cpp)
set_target_properties(${TARGET} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "ImageSegmentation.h;Line.h;Pixel.h")


add_compile_options(-Wall -pedantic -std=c++11 -O3)
add_definitions(-Dcimg_use_tiff)
target_link_libraries(${TARGET} -lX11 -lpthread -ltiff)

install(TARGETS ${TARGET}
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/imagesegmentation)




