# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/StaticGLEW_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StaticGLEW_autogen.dir/ParseCache.txt"
  "CMakeFiles/TINYXML2_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/TINYXML2_autogen.dir/ParseCache.txt"
  "CMakeFiles/final_project_playground_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/final_project_playground_autogen.dir/ParseCache.txt"
  "Simple-OpenGL-Image-Library/CMakeFiles/SOIL_autogen.dir/AutogenUsed.txt"
  "Simple-OpenGL-Image-Library/CMakeFiles/SOIL_autogen.dir/ParseCache.txt"
  "Simple-OpenGL-Image-Library/SOIL_autogen"
  "StaticGLEW_autogen"
  "TINYXML2_autogen"
  "final_project_playground_autogen"
  )
endif()
