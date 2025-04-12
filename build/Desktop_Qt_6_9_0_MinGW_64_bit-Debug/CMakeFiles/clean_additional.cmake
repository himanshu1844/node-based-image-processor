# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ImageProcessor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ImageProcessor_autogen.dir\\ParseCache.txt"
  "ImageProcessor_autogen"
  "nodeeditor\\CMakeFiles\\QtNodes_autogen.dir\\AutogenUsed.txt"
  "nodeeditor\\CMakeFiles\\QtNodes_autogen.dir\\ParseCache.txt"
  "nodeeditor\\QtNodes_autogen"
  )
endif()
