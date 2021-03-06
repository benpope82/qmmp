project(libconverter)

cmake_minimum_required(VERSION 2.4.7)

include(${QT_USE_FILE})

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmpui & libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# taglib
include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})


SET(libconverter_SRCS
    converterfactory.cpp
    converter.cpp
    converterhelper.cpp
    converterdialog.cpp
    preseteditor.cpp
)

SET(libconverter_RCCS 
    translations/translations.qrc presets.qrc)

QT4_ADD_RESOURCES(libconverter_RCC_SRCS ${libconverter_RCCS})

# user interface

SET(libconverter_UIS
    converterdialog.ui
    preseteditor.ui
)

QT4_WRAP_UI(libconverter_UIS_H ${libconverter_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(converter MODULE ${libconverter_SRCS} ${libconverter_UIS_H} ${libconverter_RCC_SRCS})
add_dependencies(converter qmmpui)
target_link_libraries(converter ${QT_LIBRARIES} qmmpui libqmmp ${TAGLIB_LDFLAGS})
install(TARGETS converter DESTINATION ${LIB_DIR}/qmmp/General)
