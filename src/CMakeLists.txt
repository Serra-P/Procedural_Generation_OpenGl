file(GLOB SRC_SOURCES
  CONFIGURE_DEPENDS
  "*.cpp"
)

add_library(src ${SRC_SOURCES})

target_include_directories(src
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(src
  PRIVATE
  ${OPENGL_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLUT_LIBRARIES}
  glfw
)
