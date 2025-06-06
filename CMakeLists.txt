cmake_minimum_required(VERSION 3.5)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(VitaMANAGER)

include("${VITASDK}/share/vita.cmake" REQUIRED)

set(VITA_APP_NAME "Vita Manager")
set(VITA_TITLEID  "VSDK00014")
set(VITA_VERSION  "02.01")


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")



add_executable(${PROJECT_NAME}
  src/main.c
  src/ctrl.c
)

target_link_libraries(${PROJECT_NAME}
	SceLibKernel_stub
	SceIofilemgr_stub
	SceAppMgr_stub
	SceCtrl_stub
	SceDisplay_stub
	SceVshBridge_stub
	ScePower_stub
	SceRegistryMgr_stub
	SceLibc_stub
  	SceThreadmgrForDriver_stub
	SceSysclibForDriver_stub
	SceCpuForDriver_stub
	SceSysmemForDriver_stub
	SceIofilemgrForDriver_stub
)


vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME} UNSAFE)
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/pic0.png sce_sys/pic0.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
