file(GLOB SRC_SOURCES
  CONFIGURE_DEPENDS
  "*.cpp"
)

add_library(imgui ${SRC_SOURCES})

target_include_directories(imgui
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(imgui
  PRIVATE
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  glfw
)
