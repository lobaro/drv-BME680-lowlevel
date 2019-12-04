project(drv-BME680-lowlevel)

include(${CPATH}/src/github.com/lobaro/c-build/build.cmake)
cbuild_module()

cbuild_dependency(_ github.com/lobaro/c-utils) # for lobaro_asssert, datetime, c stdlibs