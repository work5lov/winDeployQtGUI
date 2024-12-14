# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\winDeployQtGUI_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\winDeployQtGUI_autogen.dir\\ParseCache.txt"
  "winDeployQtGUI_autogen"
  )
endif()
