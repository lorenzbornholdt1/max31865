cmake_minimum_required(VERSION 3.13.1)
set(CONF_FILE "prj.conf")
##You can define an alternative pinmapping in an .overlayFile!
#set(DTC_OVERLAY_FILE "nucleol4.overlay") 




include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(max31865)

FILE(GLOB app_sources ../src/*.c*)
target_sources(app PRIVATE ${app_sources})
