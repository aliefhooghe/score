project(score_app LANGUAGES CXX)

score_common_setup()
score_write_static_plugins_header()
enable_testing()
set(APPNAME "score")
set(CMAKE_POSITION_INDEPENDENT_CODE 1)
# TODO strip symbols after deployment build

add_executable(${APPNAME} WIN32
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
)

target_link_libraries(${APPNAME} PUBLIC score_lib_base)
if(SCORE_STATIC_PLUGINS)
  target_link_libraries(${APPNAME} PUBLIC ${SCORE_PLUGINS_LIST})
endif()

find_package(Qt5 5.7 COMPONENTS QuickControls2)
if(TARGET Qt5::QuickControls2)
  target_link_libraries(${APPNAME} PUBLIC score_lib_base Qt5::QuickControls2)
endif()

if(MSVC)
  set_property(
    TARGET ${APPNAME}
    APPEND PROPERTY
      LINK_FLAGS "/DYNAMICBASE:NO")
  target_sources(${APPNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/score.rc")
endif()

if(UNIX AND NOT APPLE)
  # Linux package is called ossia-score
  set_target_properties(${APPNAME}
    PROPERTIES
      OUTPUT_NAME ossia-score)

  target_link_libraries(${APPNAME} PUBLIC X11)
endif()

if(APPLE)
    target_sources(${APPNAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/mac_main.m")

    find_library(CFLIB CoreFoundation)
    find_library(FLIB Foundation)
    target_link_libraries(${APPNAME} PRIVATE ${CFLIB} ${FLIB})

    if(DEPLOYMENT_BUILD)
      set(ICON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../lib/resources/score.icns")
      set_source_files_properties(${ICON_SRC} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

      target_sources(${APPNAME} PRIVATE ${ICON_SRC})
      set_target_properties(${APPNAME} PROPERTIES MACOSX_BUNDLE TRUE)
    endif()
endif()

include(StaticApp.cmake)

if(IOS)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(CORETEXT_LIBRARY CoreText)
  find_library(FOUNDATION_LIBRARY Foundation)
  find_library(ACCELERATE_LIBRARY Accelerate)
  find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox)
  find_library(SECURITY_LIBRARY Security)
  find_library(MobileCoreServices_LIBRARY MobileCoreServices)
  find_library(SystemConfiguration_LIBRARY SystemConfiguration)
  find_library(CoreGraphics_LIBRARY CoreGraphics)
  find_library(UIKit_LIBRARY UIKit)

  get_target_property(QtCore_LIB Qt5::Core LOCATION)
  get_filename_component(QT_LIB_FOLDER ${QtCore_LIB} DIRECTORY)
  find_library(qtharfbuzzng_LIBRARY NAMES qtharfbuzzng libqtharfbuzzng HINTS ${QT_LIB_FOLDER})
  find_library(qtfreetype_LIBRARY NAMES qtfreetype libqtfreetype HINTS ${QT_LIB_FOLDER})
  find_library(qtpcre_LIBRARY NAMES qtpcre libqtpcre HINTS ${QT_LIB_FOLDER})
  target_link_libraries(${APPNAME} PUBLIC ${qtharfbuzzng_LIBRARY} ${qtfreetype_LIBRARY} ${qtpcre_LIBRARY}
    ${CoreGraphics_LIBRARY} ${CORETEXT_LIBRARY} ${COREFOUNDATION_LIBRARY}
    ${COREAUDIO_LIBRARY} ${SECURITY_LIBRARY} ${FOUNDATION_LIBRARY}
    ${ACCELERATE_LIBRARY} ${AUDIOTOOLBOX_LIBRARY}
    ${MobileCoreServices_LIBRARY} ${SystemConfiguration_LIBRARY} ${UIKit_LIBRARY}
    )
endif()

setup_score_common_exe_features(${APPNAME})

if(WIN32)
  install(
    TARGETS ${APPNAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION .)
else()
  install(
    TARGETS ${APPNAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin)
endif()

include(UnityApp.cmake)
include(IScoreDeployment)
