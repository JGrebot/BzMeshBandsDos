set(HEADER_FILES_LIBBZMESH
    mesh_tetra.hpp
    mesh_vertex.hpp
    bz_mesh.hpp
    vector.hpp 
    iso_triangle.hpp)

set(SOURCE_FILES_LIBBZMESH 
    mesh_tetra.cpp
    bz_mesh.cpp)

add_library(lib_bzmesh STATIC ${SOURCE_FILES_LIBBZMESH} ${HEADER_FILES_LIBBZMESH})

target_include_directories(lib_bzmesh PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Link with reuired libraries
target_link_libraries(lib_bzmesh PUBLIC pthread Eigen3::Eigen)
target_link_libraries(lib_bzmesh PUBLIC ${GMSH_LIB})
target_include_directories(lib_bzmesh PUBLIC ${GMSH_INC})
if(USE_OPENMP_ACCELERATION)
  target_link_libraries(lib_bzmesh PUBLIC OpenMP::OpenMP_CXX)
endif(USE_OPENMP_ACCELERATION)

# All users of this library will need at least C++20
target_compile_features(lib_bzmesh PRIVATE cxx_std_20)