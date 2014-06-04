#--------------------------------------------------------------------------------------------
# Root Cmake file for MIS&R project
# Version 1.0
# Date 29.06.2011
# Author: Kamil Wereszczynski
#--------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.6)

project(CVL)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")                
        set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")     

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH}")
        
#set (CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
message (STATUS "CMAKE_EXE_LINKER_FLAGS:" ${CMAKE_EXE_LINKER_FLAGS})
#project choice

option (USE_DATAMAN "KW: Application for data management" ON)



#set (CMAKE_CXX_COMPILER "/usr/bin/gcc-4.5")
#set (CMAKE_CXX_FLAGS "-std=c++0x")

#project destination paths:
if (USE_DATAMAN)
		set (DEST_DATAMAN "${CMAKE_BINARY_DIR}/dataMan" CACHE PATH "Output destination for data management application")
endif(USE_DATAMAN)

		
# set opencv paths
set(CV_INCLUDE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/openCV/opencv"  
		"${CMAKE_CURRENT_SOURCE_DIR}/include/openCV/opencv2"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/openCV"
        )
set(CV_LIB_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/OpenCV"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/OpenCV"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/OpenCV"
        )


		
set(CV_DEBUG_LIBS_BASIC 
        debug opencv_core231d
        debug opencv_highgui231d
        debug opencv_video231d
        debug opencv_ml231d
        debug opencv_legacy231d
	debug opencv_objdetect231d
	debug opencv_flann231d)  #dodane


set(CV_DEBUG_LIBS_CALIBRATION
        debug opencv_features2d231d
        debug opencv_calib3d231d
        debug opencv_imgproc231d)
        

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CV_RELEASE_LIBS_BASIC
                optimized libopencv_core.so
                optimized libopencv_highgui.so
                optimized libopencv_video.so
                optimized libopencv_ml.so
                optimized libopencv_legacy.so
				optimized libopencv_objdetect.so
				optimized libopencv_flann.so#dodane
                )
        set(CV_RELEASE_LIBS_CALIBRATION
                optimized opencv_features2d.so
                optimized opencv_calib3d.so
                optimized opencv_imgproc.so	
		)    
        SET(OperatingSystem "Linux")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(CV_RELEASE_LIBS_BASIC
                optimized opencv_core231
                optimized opencv_highgui231
                optimized opencv_video231.lib
                optimized opencv_ml231.lib
                optimized opencv_legacy231.lib
				optimized opencv_objdetect231.lib
				optimized opencv_flann231)
                
        set(CV_RELEASE_LIBS_CALIBRATION
                optimized opencv_features2d231
                optimized opencv_calib3d231
                optimized opencv_imgproc231)
        SET(OperatingSystem "Windows")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")           

set(CV_DLL_DEBUG_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/opencv")

set(CV_DLL_RELEASE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/opencv")
        
# set curl libs and copy dll to output
set(CURL_INCLUDE_PATH   
        "${CMAKE_CURRENT_SOURCE_DIR}/include/curl"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/curl/curl") 
set(CURL_LIB_PATH               
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/curl"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/curl"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/curl"
        )
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        
        set(CURL_RELEASE_LIBS
                libcurl_imp.lib)
        set(CURL_DEBUG_LIBS
                libcurld_imp.lib)

        set(CURL_DLL_DEBUG_PATH 
                "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/curl")

        set(CURL_DLL_RELEASE_PATH 
                "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/curl")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(CURL_RELEASE_LIBS
                libcurl.so)
        set(CURL_DEBUG_LIBS
                libcurl.so)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        
#set include paths and libs for openThreads

set(TH_INCLUDE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/openThreads")

set(TH_LIB_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/OpenThreads"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/OpenThreads"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/OpenThreads"
        )

set(TH_DLL_DEBUG_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/OpenThreads")

set(TH_DLL_RELEASE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/OpenThreads")
        
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(TH_DEBUG_LIBS 
                debug OpenThreadsWin32d)

        set(TH_RELEASE_LIBS 
                optimized OpenThreadsWin32)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(TH_DEBUG_LIBS 
                debug libOpenThreads.so)

        set(TH_RELEASE_LIBS 
                optimized libOpenThreads.so)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#set include paths and libs for boost
set(BOOST_INCLUDE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/boost")

set(BOOST_LIB_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/boost"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/boost"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/boost"
        )

set(BOOST_DLL_DEBUG_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/boost")

set(BOOST_DLL_RELEASE_PATH 
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/boost")
        
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(BOOST_DEBUG_LIBS_FILESYSTEM 
                debug boost_filesystem-vc100-mt-gd-1_47
                debug libboost_filesystem-vc100-mt-gd-1_47
                debug libboost_filesystem-vc100-mt-sgd-1_47
                debug libboost_filesystem-vc100-sgd-1_47
                )

        set(BOOST_RELEASE_LIBS_FILESYSTEM
                optimized boost_filesystem-vc100-mt-1_47
                optimized libboost_filesystem-vc100-mt-1_47
                optimized libboost_filesystem-vc100-mt-s-1_47
                optimized libboost_filesystem-vc100-s-1_47
                
                )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

        set(BOOST_DEBUG_LIBS_FILESYSTEM
				debug libboost_filesystem.a
				debug libboost_system.a
				debug libboost_program_options.a)
				
        set(BOOST_RELEASE_LIBS_FILESYSTEM
                optimized libboost_filesystem.a
				optimized libboost_system.a
				optimized libboost_program_options.a)
				
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(BOOST_DEBUG_LIBS_SERIALIZATION
                debug boost_serialization-vc100-mt-gd-1_47
                debug libboost_serialization-vc100-mt-gd-1_47
                debug libboost_serialization-vc100-mt-sgd-1_47
                debug libboost_serialization-vc100-sgd-1_47
                )

        set(BOOST_RELEASE_LIBS_SERIALIZATION
                optimized boost_serialization-vc100-mt-1_47
                optimized libboost_serialization-vc100-mt-1_47
                optimized libboost_serialization-vc100-mt-s-1_47
                optimized libboost_serialization-vc100-s-1_47
                
                )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

        set(BOOST_DEBUG_LIBS_SERIALIZATION
				debug libboost_serialization.a)
				
        set(BOOST_RELEASE_LIBS_SERIALIZATION
                optimized libboost_serialization.a)
				
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#set path to qt files needed for openCV

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		set(QT_INCLUDE_PATH 
			"${CMAKE_CURRENT_SOURCE_DIR}/include/qt/include")
			
		set (QT_DEBUG_LIBS
				debug QtCored4
				debug QtGuid4
				debug QtOpenGLd4
				debug QtTestd4
			)
			
		set (QT_RELEASE_LIBS
				optimized QtCore4
				optimized QtGui4
				optimized QtOpenGL4
				optimized QtTest4
			)
			
		set(QT_LIB_PATH 
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/qt"
			"${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/qt"
			)
        set(QT_DLL_DEBUG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Debug/qt")
        set(QT_DLL_RELEASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/win32/Release/qt")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

add_subdirectory(src)
