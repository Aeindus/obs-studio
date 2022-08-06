cmake_minimum_required(VERSION 3.16...3.21)

project(aeindus-custom)
add_library(${PROJECT_NAME} MODULE)

# Replace `Your Name Here` with the name (yours or your organization's) you want
# to see as the author of the plugin (in the plugin's metadata itself and in the
# installers)
set(PLUGIN_AUTHOR "Aeindus")
set(MODULE_DESCRIPTION "Aeindus Custom Behaviour Module")

# Add your custom source files here - header files are optional and only
# required for visibility e.g. in Xcode or Visual Studio
target_sources(${PROJECT_NAME} PRIVATE plugin-main.cpp util.cpp util.hpp 
                ZoomManager.cpp ZoomManager.hpp JWManager.cpp JWManager.hpp)

# Generate custom header file with the plugin's details
configure_file(plugin-macros.hpp.in
               ${CMAKE_CURRENT_SOURCE_DIR}/plugin-macros.generated.hpp)
target_sources(${PROJECT_NAME} PRIVATE plugin-macros.generated.hpp)

# --- Platform-independent build settings ---
target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE OBS::libobs OBS::frontend-api Dwmapi.lib)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)


if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /MP /d2FH4-)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "plugins" PREFIX "")
setup_plugin_target(${PROJECT_NAME})
