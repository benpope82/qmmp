project(app)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
   cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy) 

SET(QT_USE_QTNETWORK TRUE)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

include(${QT_USE_FILE})

ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# libqmmp & libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_INSTALL_PREFIX}/${LIB_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmp)

SET(app_SRCS
    builtincommandlineoption.cpp
    main.cpp
    qmmpstarter.cpp
    lxdesupport.cpp
    qmmpapplication.cpp
)

SET(app_HDRS lxdesupport.h)

SET(app_MOC_HDRS
    builtincommandlineoption.h
    qmmpstarter.h
    qmmpapplication.h
)

SET(app_RCCS images/images.qrc translations/qmmp_locales.qrc)

QT4_ADD_RESOURCES(app_RCC_SRCS ${app_RCCS})

QT4_AUTOMOC(${app_MOC_SRC})
QT4_WRAP_CPP(app_MOC_SRCS ${app_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(qmmp ${app_SRCS} ${app_MOC_SRCS} ${app_RCC_SRCS} ${app_HDRS})
target_link_libraries(qmmp ${QT_LIBRARIES} libqmmp qmmpui)
add_dependencies(qmmp qmmpui libqmmp)
install(TARGETS qmmp DESTINATION bin)
install(FILES qmmp.desktop DESTINATION share/applications)
install(FILES images/16x16/qmmp.png DESTINATION share/icons/hicolor/16x16/apps)
install(FILES images/32x32/qmmp.png DESTINATION share/icons/hicolor/32x32/apps)
install(FILES images/48x48/qmmp.png DESTINATION share/icons/hicolor/48x48/apps)
install(FILES images/scalable/qmmp.svgz images/scalable/qmmp-simple.svgz DESTINATION share/icons/hicolor/scalable/apps)

IF(USE_DIR_ASSOC)
install(FILES qmmp_enqueue.desktop DESTINATION share/applications)
install(FILES qmmp_dir.desktop DESTINATION share/applications)
ENDIF(USE_DIR_ASSOC)