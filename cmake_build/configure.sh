#!/bin/sh

# additional compiler flags could be added customizing the corresponding var, for example
# -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -msse3 ". Note that the "defaults are already correctly coded"
#so we should ask here only machine specific stuff

#an effort is made to autodetect all of the libraries needed HOWEVER:
#METIS_APPLICATION needs the var PARMETIS_ROOT_DIR to be specified by the user (not needed if the app is set to OFF)
#TRILINOS_APPLICATION needs the var PARMETIS_ROOT_DIR to be specified by the user (not needed if the app is set to OFF)
#MKL_SOLVERS_APPLICATION needs the var MKLSOLVER_INCLUDE_DIR and MKLSOLVER_LIB_DIR to be specified by the user (not needed if the app is set to OFF)
#note that the MKLSOLVER_LIB_DIR should include /lib/em64t. This is needed as intel is changing location of mkl at every update of the compiler!!

#the user should also note that the symbol "\" marks that the command continues on the next line. IT SHOULD ONLY BE FOLLOWED
#BY the "ENTER" and NOT by any space!!

clear

#you may want to decomment this the first time you compile
rm CMakeCache.txt
rm *.cmake
rm -rf CMakeFiles\

cmake ..                                                                      \
-DCMAKE_C_COMPILER=/usr/bin/gcc                                               \
-DCMAKE_INSTALL_RPATH="$HOME/opt/kratos/libs"                                 \
-DCMAKE_INSTALL_RPATH_USE_LINK_PATH=TRUE                                      \
-DCMAKE_CXX_COMPILER=/usr/bin/g++                                             \
-DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -msse3 -std=c++11 "                     \
-DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -msse3 "                                    \
-DPYTHON_LIBRARY="/usr/lib/python2.7/config-x86_64-linux-gnu/libpython2.7.so" \
-DPYTHON_INCLUDE_DIR="/usr/include/python2.7"                                 \
-DCMAKE_BUILD_TYPE=Release                                                    \
-DINSTALL_EMBEDDED_PYTHON=ON                                                  \
\
-DEXTERNAL_SOLVERS_APPLICATION=ON                                             \
-DINCLUDE_FEAST=ON                                                            \
\
-DSOLID_MECHANICS_APPLICATION=ON                                              \
-DSTRUCTURAL_MECHANICS_APPLICATION=ON                                         \
-DMESHING_APPLICATION=ON                                                      \
-DSHAPE_OPTIMIZATION_APPLICATION=ON                                           \
-DTOPOLOGY_OPTIMIZATION_APPLICATION=ON                                        \
-DALE_APPLICATION=ON                                                          \
\
-DMULTI_BODY_DYNAMICS_APPLICATION=ON                                          \
\
-DTEST_APPLICATION=OFF                                                        \
-DPURE_DIFFUSION_APPLICATION=ON                                               \
\
-DADJOINT_FLUID_APPLICATION=ON                                                \
-DCONSTITUTIVE_LAWS_APPLICATION=ON                                            \
-DCONVECTION_DIFFUSION_APPLICATION=ON                                         \
-DCONTACT_MECHANICS_APPLICATION=ON                                            \
-DCONTACT_STRUCTURAL_MECHANICS_APPLICATION=ON                                 \
-DCONVECTION_DIFFUSION_APPLICATION=ON                                         \
-DDEM_APPLICATION=ON                                                          \
-DEMPIRE_APPLICATION=ON                                                       \
-DFLUID_DYNAMICS_APPLICATION=ON                                               \
-DFSI_APPLICATION=ON                                                          \
-DINCOMPRESSIBLE_FLUID_APPLICATION=ON                                         \
-DMAPPING_APPLICATION=ON                                                      \
-DMIXED_ELEMENT_APPLICATION=ON                                                \
-DPFEM_BASE_APPLICATION=ON                                                    \
-DPFEM_SOLID_MECHANICS_APPLICATION=ON                                         \
-DSTRUCTURAL_APPLICATION=ON                                                   \
-DSWIMMING_DEM_APPLICATION=ON                                                 \
-DTHERMO_MECHANICAL_APPLICATION=ON                                            \
-DWIND_TURBINE_APPLICATION=ON                                                 \
\
-DIGA_STRUCTURAL_MECHANICS_APPLICATION=OFF                                    \
-DNURBS_APPLICATION=OFF                                                       \
-DMULTISCALE_APPLICATION=OFF                                                  \
-DOPENCL_APPLICATION=OFF                                                      \
\
-DCOMPRESSIBLE_POTENTIAL_FLOW_APPLICATION=OFF                                 \
-DDAM_APPLICATION=OFF                                                         \
-DDEM_FEM_APPLICATION=OFF                                                     \
-DFREEZING_SOIL_APPLICATION=OFF                                               \
-DMESHLESS_APPLICATION=OFF                                                    \
-DMPI_SEARCH_APPLICATION=OFF                                                  \
-DLAGRANGIAN_MPM_APPLICATION=OFF                                              \
-DPARTICLE_MECHANICS_APPLICATION=OFF                                          \
-DPFEM_APPLICATION=OFF                                                        \
-DPFEM_FLUID_DYNAMICS_APPLICATION=OFF                                         \
-DPOROMECHANICS_APPLICATION=OFF                                               \
-DSTRING_DYNAMICS_APPLICATION=OFF                                             \
-DULF_APPLICATION=OFF                                                         \
\
-DBLOOD_FLOW_APPLICATION=OFF                                                  \
-DCLICK2CAST_APPLICATION=OFF                                                  \
-DCONSTITUTIVE_MODELS_APPLICATION=OFF                                         \
-DFORMING_APPLICATION=OFF                                                     \
-DMACHINING_APPLICATION=OFF                                                   \
-DMY_LAPLACIAN_APPLICATION=OFF                                                \
-DPFEM2_APPLICATION=OFF                                                       \
-DTURBULENT_FLOW_APPLICATION=OFF                                              \
\
-DMKL_SOLVERS_APPLICATION=OFF                                                 \
-DMKLSOLVER_INCLUDE_DIR="$HOME/opt/intel/Compiler/11.1/072/mkl/include"       \
-DMKLSOLVER_LIB_DIR="$HOME/opt/intel/Compiler/11.1/072/mkl/lib/em64t"         \
-DMETIS_APPLICATION=OFF                                                       \
-DPARMETIS_ROOT_DIR="$HOME/compiled_libraries/ParMetis-3.1.1"                 \
-DTRILINOS_APPLICATION=OFF                                                    \
-DTRILINOS_ROOT="$HOME/compiled_libraries/trilinos-10.2.0"                    \
\
#-DCMAKE_BUILD_TYPE=Debug                                                      \
-DCMAKE_BUILD_TYPE=RelWithDebInfo                                             \
#-DKRATOS_DEBUG=ON                                                             \
#-DDO_NOT_DEFINE_NDEBUG=ON                                                     \


#decomment this to have it verbose
# make VERBOSE=1 -j4
make -j2
make install
