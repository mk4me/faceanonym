message("Data Manager:")
project(dataMan)
SET(EXECUTABLE_OUTPUT_PATH ${DEST_DATAMAN})
# general includes and links
include_directories(${CV_INCLUDE_PATH})
include_directories(${BOOST_INCLUDE_PATH})
include_directories(${TH_INCLUDE_PATH})
include_directories(${QT_INCLUDE_PATH})

link_directories(${CV_LIB_PATH})
message(${CV_LIB_PATH})
link_directories(${BOOST_LIB_PATH})
link_directories(${TH_LIB_PATH})
link_directories(${QT_LIB_PATH})

# target
set(SOURCES
	main.cpp
	Anonymization.cpp
	folderOrganize.cpp
	faceAnonymizer.cpp
	trainingData.cpp
	qtanonimizationform.cpp
	moc_qtanonimizationform.cpp
	createTrainingSet.cpp
	imageList.cpp
	imageViewer.cpp
)
 
set(HEADERS
	main.h
	Anonymization.h
	folderOrganize.h
	faceAnonymizer.h
	trainingData.h
	qtanonimizationform.h
	ui_qtanonimizationform.h
	createTrainingSet.h
	imageList.h
	imageViewer.h
)
 
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})

add_executable(dataMan ${SOURCES} ${HEADERS})

target_link_libraries(dataMan ${CV_DEBUG_LIBS_BASIC} ${CV_RELEASE_LIBS_BASIC})
target_link_libraries(dataMan ${CV_DEBUG_LIBS_CALIBRATION} ${CV_RELEASE_LIBS_CALIBRATION})
target_link_libraries(dataMan ${BOOST_DEBUG_LIBS_FILESYSTEM} ${BOOST_RELEASE_LIBS_FILESYSTEM})
target_link_libraries(dataMan ${ZLIB_DEBUG_LIB} ${ZLIB_RELEASE_LIB})
target_link_libraries(dataMan ${TH_DEBUG_LIBS} ${TH_RELEASE_LIBS})
target_link_libraries(dataMan ${QT_DEBUG_LIBS} ${QT_RELEASE_LIBS})

#installing openCV's necessary DLLs, PDBs and ILKs:
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_core231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_core231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_highgui231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_highgui231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_video231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_video231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_ml231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_ml231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_legacy231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_legacy231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_imgproc231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_imgproc231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_calib3d231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_calib3d231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_features2d231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_features2d231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_flann231.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_flann231.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${CV_DLL_RELEASE_PATH}/opencv_ffmpeg.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${QT_DLL_RELEASE_PATH}/QtCore4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${QT_DLL_RELEASE_PATH}/QtGui4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${QT_DLL_RELEASE_PATH}/QtOpenGL4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${QT_DLL_RELEASE_PATH}/QtTest4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)


install(FILES "${CV_DLL_DEBUG_PATH}/opencv_core231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_core231d.pdb" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_highgui231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_highgui231d.ilk" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_video231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_ml231d.lib" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_legacy231d.lib" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_imgproc231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_imgproc231d.ilk" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_calib3d231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_calib3d231d.ilk" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_features2d231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_features2d231d.ilk" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_flann231d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_flann231d.ilk" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${CV_DLL_DEBUG_PATH}/opencv_ffmpeg.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${QT_DLL_DEBUG_PATH}/QtCored4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${QT_DLL_DEBUG_PATH}/QtGuid4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${QT_DLL_DEBUG_PATH}/QtOpenGLd4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${QT_DLL_DEBUG_PATH}/QtTestd4.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)

install(FILES "${BOOST_DLL_DEBUG_PATH}/boost_filesystem-vc100-mt-gd-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${BOOST_DLL_DEBUG_PATH}/boost_system-vc100-mt-gd-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${BOOST_DLL_DEBUG_PATH}/boost_serialization-vc100-mt-gd-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)
install(FILES "${BOOST_DLL_RELEASE_PATH}/boost_system-vc100-mt-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${BOOST_DLL_RELEASE_PATH}/boost_filesystem-vc100-mt-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${BOOST_DLL_RELEASE_PATH}/boost_serialization-vc100-mt-1_47.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)

#install(FILES "${CURL_DLL_DEBUG_PATH}/libcurld.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
#install(FILES "${CURL_DLL_DEBUG_PATH}/libcurld.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)

install(FILES "${TH_DLL_RELEASE_PATH}/OpenThreadsWin32.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Release)
install(FILES "${TH_DLL_DEBUG_PATH}/OpenThreadsWin32d.dll" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Debug)

message("done.")