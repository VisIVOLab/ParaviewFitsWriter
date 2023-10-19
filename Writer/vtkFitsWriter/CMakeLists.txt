# find_package(Boost REQUIRED)
find_package(CFITSIO REQUIRED)

SET(classes
  vtkFitsWriter)

vtk_module_add_module(FitsWriter::vtkFitsWriter
  FORCE_STATIC # Using FORCE_STATIC build the vtk module statically into the plugin library, to avoid confusion when loading                      
  CLASSES ${classes})

paraview_add_server_manager_xmls(
  XMLS  writers.xml)

target_include_directories(vtkFitsWriter PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(vtkFitsWriter PRIVATE CFITSIO::CFITSIO)
