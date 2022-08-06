cmake_minimum_required(VERSION 3.20)
project(win-capture-audio VERSION 2.2.3)

# Install plugin into destination folder
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/obs-plugins/
  DESTINATION "./obs-plugins")
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION "./data")
  
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/obs-plugins/
  DESTINATION "./obs-plugins"
  COMPONENT obs_rundir)
install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/
  DESTINATION "./data"
  COMPONENT obs_rundir)
