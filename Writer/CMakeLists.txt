paraview_add_plugin(FitsWriter
  REQUIRED_ON_SERVER
  VERSION "1.0"
  MODULES FitsWriter::vtkFitsWriter
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/vtkFitsWriter/vtk.module"
)
