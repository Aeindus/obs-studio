cmake_minimum_required(VERSION 3.16...3.21)

project(aeindus-custom)
add_library(${PROJECT_NAME} MODULE)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the
# installers)
set(PLUGIN_AUTHOR "Aeindus")


# This macro seems to be already set in cmake/Modules/ObsHelpers.cmake
# But if you look closely, all core plugins use the definition below directly in their cmake file
macro(find_qt)
  set(oneValueArgs VERSION)
  set(multiValueArgs COMPONENTS COMPONENTS_WIN COMPONENTS_MAC COMPONENTS_LINUX)
  cmake_parse_arguments(FIND_QT "" "${oneValueArgs}" "${multiValueArgs}"
                        ${ARGN})

  if(OS_WINDOWS)
    find_package(
      Qt${FIND_QT_VERSION}
      COMPONENTS ${FIND_QT_COMPONENTS} ${FIND_QT_COMPONENTS_WIN}
      REQUIRED)
  elseif(OS_MACOS)
    find_package(
      Qt${FIND_QT_VERSION}
      COMPONENTS ${FIND_QT_COMPONENTS} ${FIND_QT_COMPONENTS_MAC}
      REQUIRED)
  else()
    find_package(
      Qt${FIND_QT_VERSION}
      COMPONENTS ${FIND_QT_COMPONENTS} ${FIND_QT_COMPONENTS_LINUX}
      REQUIRED)
  endif()

  foreach(_COMPONENT IN LISTS FIND_QT_COMPONENTS FIND_QT_COMPONENTS_WIN
                              FIND_QT_COMPONENTS_MAC FIND_QT_COMPONENTS_LINUX)
    if(NOT TARGET Qt::${_COMPONENT} AND TARGET
                                        Qt${FIND_QT_VERSION}::${_COMPONENT})

      add_library(Qt::${_COMPONENT} INTERFACE IMPORTED)
      set_target_properties(
        Qt::${_COMPONENT} PROPERTIES INTERFACE_LINK_LIBRARIES
                                     "Qt${FIND_QT_VERSION}::${_COMPONENT}")
    endif()
  endforeach()
endmacro()

# Add your custom source files here - header files are optional and only
# required for visibility e.g. in Xcode or Visual Studio
target_sources(${PROJECT_NAME} PRIVATE plugin-main.cpp util.cpp util.hpp)

# /!\ TAKE NOTE: No need to edit things past this point /!\

if(NOT QT_VERSION)
  set(QT_VERSION
      "5"
      CACHE STRING "OBS Qt version [5, 6]" FORCE)
  set_property(CACHE QT_VERSION PROPERTY STRINGS 5 6)
endif()

find_qt(VERSION ${QT_VERSION} COMPONENTS Widgets Core)

configure_file(plugin-macros.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/plugin-macros.generated.hpp)

target_sources(${PROJECT_NAME} PRIVATE plugin-macros.generated.hpp)

# --- Platform-independent build settings ---

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME} PRIVATE OBS::libobs OBS::frontend-api Qt::Core Qt::Widgets)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# --- End of section ---

# --- Windows-specific build settings and tasks ---
if(OS_WINDOWS)
  set(MODULE_DESCRIPTION "Aeindus Custom Behaviour Module")

  if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /d2FH4-)
  endif()
  # --- End of section ---
endif()
# --- End of section ---

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "plugins" PREFIX "")
setup_plugin_target(${PROJECT_NAME})
