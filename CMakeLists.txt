cmake_minimum_required(VERSION 3.28.0)

list(
    APPEND
        EXTRA_CONF_FILE
           ${CMAKE_CURRENT_SOURCE_DIR}/embedded-pio/zephyrcan/zephyrcan.conf
           ${CMAKE_CURRENT_SOURCE_DIR}/embedded-pio/updatemanager_eth/updatemanager_eth.conf
)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(PDC LANGUAGES C CXX)

target_include_directories(app PRIVATE include)


file(GLOB APP_SOURCES CONFIGURE_DEPENDS src/*.cpp)

target_sources(app PRIVATE ${APP_SOURCES})


 add_subdirectory(embedded-pio/zephyrcan)
  add_subdirectory(embedded-pio/updatemanager_eth)


 

target_link_libraries(app
      PRIVATE
          zephyrcan
          updatemanager_eth
  )




