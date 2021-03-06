## http://www.cmake.org/cmake/help/v2.8.12/cmake.html#module:FindQt4
## http://www.cmake.org/cmake/help/v3.0/manual/cmake-qt.7.html#introduction

cmake_minimum_required (VERSION 2.8)
project (testfem)
set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_BUILD_TYPE RELEASE)
include_directories ("../shared" 
"../cudabvh" 
"../radixsort" 
"../cudanarrowphase"
"../cudadyn"
"../hesperis"
"../cudafem")

set (HEADER_FILES "../shared/Base3DView.h" 
"../shared/Morton3D.h"
"../shared/BaseSolverThread.h"
"../shared/CudaBase.h"
"../shared/BaseBuffer.h"
"../shared/CUDABuffer.h"
"../shared/ScanUtil.h"
"../shared/CudaDbgLog.h"
"../cudabvh/CudaLinearBvh.h"
"../cudabvh/TetrahedronSystem.h"
"../cudabvh/CudaTetrahedronSystem.h"
"../cudabvh/createBvh_implement.h"
"../cudabvh/reduce_common.h"
"../cudabvh/reduceBox_implement.h"
"../cudabvh/reduceRange_implement.h"
"../cudabvh/createBvh_implement.h"
"../cudabvh/tetrahedronSystem_implement.h"
"../cudabvh/scan_implement.h"
"../radixsort/radixsort_implement.h"
"../cudabvh/CudaBroadphase.h"
"../cudabvh/CudaReduction.h"
"../cudanarrowphase/CudaNarrowphase.h"
"../cudanarrowphase/narrowphase_implement.h"
"../cudanarrowphase/simpleContactSolver_implement.h"
"../cudanarrowphase/SimpleContactSolver.h"
"../cudadyn/CudaDynamicWorld.h"
"../cudadyn/DynamicWorldInterface.h"
"../fem/tetmesh.h"
"../shared/CSRMatrix.h"
"../cudafem/CudaCSRMatrix.h"
"../cudafem/cuFemTetrahedron_implement.h"
"../cudafem/FEMTetrahedronSystem.h"
"../cudafem/FEMWorldInterface.h"
"../cudafem/cuConjugateGradient_implement.h"
"../cudafem/CudaConjugateGradientSolver.h"
"../cudafem/FemGlobal.h"
"../hesperis/HesperisFile.h"
"FEMTetrahedronMesh.h"
"ConjugateGradientSolver.h"
"SolverThread.h"
"window.h"
"glwidget.h")

set (SOURCE_FILES "../shared/Base3DView.cpp"
"../shared/BaseSolverThread.cpp"
"../shared/CudaBase.cpp"
"../shared/BaseBuffer.cpp"
"../shared/CUDABuffer.cpp"
"../shared/ScanUtil.cpp"
"../shared/CudaDbgLog.cpp"
"../cudabvh/CudaLinearBvh.cpp"
"../cudabvh/TetrahedronSystem.cpp"
"../cudabvh/CudaTetrahedronSystem.cpp"
"../cudabvh/CudaBroadphase.cpp"
"../cudanarrowphase/CudaNarrowphase.cpp"
"../cudanarrowphase/SimpleContactSolver.cpp"
"../cudadyn/DynGlobal.cpp"
"../cudadyn/CudaDynamicWorld.cpp"
"../cudadyn/DynamicWorldInterface.cpp"
"../cudabvh/CudaReduction.cpp"
"../shared/CSRMatrix.cpp"
"../cudafem/CudaCSRMatrix.cpp"
"../cudafem/FEMTetrahedronSystem.cpp"
"../cudafem/FEMWorldInterface.cpp"
"../cudafem/CudaConjugateGradientSolver.cpp"
"../cudafem/FemGlobal.cpp"
"../hesperis/HesperisFile.cpp"
"glwidget.cpp"
"FEMTetrahedronMesh.cpp"
"ConjugateGradientSolver.cpp"
"SolverThread.cpp"
"window.cpp"
"main.cpp")

if (WIN32)
set (SOURCE_FILES ${SOURCE_FILES} "../shared/gExtension.cpp")
endif ()

## add_executable (testcudafem ${HEADER_FILES} ${SOURCE_FILES})

find_package (CUDA QUIET REQUIRED)

message ("cuda found ? " ${CUDA_FOUND})
message ("cuda version " ${CUDA_VERSION})
message ("cuda sdk root dir " ${CUDA_SDK_ROOT_DIR})
message ("cuda librarie " ${CUDA_CUDA_LIBRARY})
message ("cuda art librarie " ${CUDA_CUDART_LIBRARY})
message ("cuda blas librarie " ${CUDA_cublas_LIBRARY})
message ("cuda sparse librarie " ${CUDA_cusparse_LIBRARY})

set (CUSOURCES "../cudabvh/bvh_math.cu"
"../cudabvh/scan.cu"
"../cudabvh/createBvh.cu"
"../cudabvh/cuReduceFlt2.cu"
"../cudabvh/cuReduceFlt3.cu"
"../cudabvh/cuReduceFlt4.cu"
"../cudabvh/reduceRange.cu"
"../cudabvh/tetrahedronSystem.cu"
"../radixsort/radixsort.cu"
"../cudabvh/broadphase.cu"
"../cudabvh/cuReduceSum.cu"
"../cudanarrowphase/stripedModel.cu"
"../cudanarrowphase/matrix_math.cu"
"../cudanarrowphase/barycentric.cu"
"../cudanarrowphase/gjk_math.cu"
"../cudanarrowphase/simpleContactSolver.cu"
"../cudanarrowphase/narrowphase.cu"
"../cudafem/cuFemMath.cu"
"../cudafem/cuFemTetrahedron.cu"
"../cudafem/cuConjugateGradient.cu")

set (CUDA_NVCC_FLAGS "--ptxas-options=-v -arch sm_11")

CUDA_ADD_EXECUTABLE (testfem ${HEADER_FILES} ${SOURCE_FILES} ${CUSOURCES})

## find_package(HDF5 REQUIRED)
if (WIN32)
set (HDF5_INCLUDE_DIR "D:/usr/hdf5/include")
set (HDF5_LIBRARIES "D:/usr/hdf5/lib/hdf5.lib")
else ()
set (HDF5_INCLUDE_DIR "/users/jianzhang/Library/hdf5/include")
set (HDF5_LIBRARIES "/users/jianzhang/Library/hdf5/lib/libhdf5.dylib")
endif ()

if (WIN32)
set (SZIP_INCLUDE_DIR "D:/usr/szip-2.1/src")
set (SZIP_LIBRARIES "D:/usr/szip-2.1/szip.lib")
include_directories (${SZIP_INCLUDE_DIR})
target_link_libraries (testfem ${SZIP_LIBRARIES})
endif ()

include_directories (${HDF5_INCLUDE_DIR})
target_link_libraries (testfem ${HDF5_LIBRARIES})

include_directories (${CUDA_TOOLKIT_INCLUDE} ${CUDA_SDK_ROOT_DIR}/common/inc)
## target_link_libraries(testfem ${CUDA_CUDA_LIBRARY})
## target_link_libraries(testfem ${CUDA_CUDART_LIBRARY})
## target_link_libraries(testfem ${CUDA_cublas_LIBRARY})
## target_link_libraries(testfem ${CUDA_cusparse_LIBRARY})

find_package (OpenGL REQUIRED)
target_link_libraries (testfem ${OPENGL_LIBRARIES})

if (WIN32)
set (Aphid_Library "D:/aphid/lib/aphid.lib")
else ()
set (Aphid_Library "/users/jianzhang/aphid/lib/libaphid.a")
endif ()

target_link_libraries (testfem ${Aphid_Library})

if (WIN32)
set (OpenEXR_Library "D:/usr/openEXR/lib/Half.lib"
"D:/usr/openEXR/lib/Iex.lib"
"D:/usr/openEXR/lib/IlmImf.lib"
"D:/usr/openEXR/lib/IlmThread.lib")
else ()
set (OpenEXR_Library "/usr/local/lib/libHalf.a"
"/usr/local/lib/libIex.a"
"/usr/local/lib/libIlmImf.a"
"/usr/local/lib/libIlmThread.a")
endif ()
target_link_libraries (testfem ${OpenEXR_Library})

if(WIN32)
set (ZLIB_ROOT "D:/usr/zlib-1.2.5")
endif()

find_package (zlib REQUIRED)
target_link_libraries (testfem ${ZLIB_LIBRARIES})

if(WIN32)
set (QT_QMAKE_EXECUTABLE "D:/usr/Qtx64/4.7.1/bin/qmake.exe")
endif()

find_package (Qt4 REQUIRED)
message ("qt include dir " ${QT_INCLUDE_DIR})
message ("qt opengl " ${QT_QTOPENGL_LIBRARY})
set (QT_MT_REQUIRED ON)
set (QT_USE_QTOPENGL ON)

include_directories (${QT_INCLUDE_DIR})
include (${QT_USE_FILE})

## target_link_libraries (testcudafem Qt4::QtGui Qt4::QtOpenGL)
target_link_libraries(testfem ${QT_LIBRARIES})

if(WIN32)
	set (Boost_INCLUDE_DIR "D:/usr/boost_1_51_0")
	set (Boost_LIBRARY_DIR "D:/usr/boost_1_51_0/stage/lib")
	set (Boost_USE_STATIC_LIBS ON)
	set (Boost_USE_MULTITHREADED ON)
	include_directories ("D:/usr/boost_1_51_0")
	find_package(Boost 1.51 COMPONENTS system filesystem date_time regex thread chrono)
else()

if(APPLE)
	set (Boost_INCLUDE_DIR "/users/jianzhang/Library/boost_1_55_0")
	set (Boost_LIBRARY_DIR "/users/jianzhang/Library/boost_1_55_0/stage/lib")
	include_directories ("/users/jianzhang/Library/boost_1_55_0")
	find_package(Boost 1.55 COMPONENTS system filesystem date_time regex thread)
	include_directories ("/users/jianzhang/Library/boost_1_55_0")

else()
	set (Boost_INCLUDE_DIR "/home/OF3D/zhangjian/Documents/boost_1_51_0")
	set (Boost_USE_STATIC_LIBS ON)
	set (Boost_USE_MULTITHREADED ON)
	include_directories ("/home/OF3D/zhangjian/Documents/boost_1_51_0")
	find_package(Boost 1.51 COMPONENTS system filesystem date_time regex thread)	
endif()
	
endif()

message ("boost system is" ${Boost_SYSTEM_LIBRARY})
message ("boost date_time is" ${Boost_DATE_TIME_LIBRARY})
message ("boost regex is" ${Boost_REGEX_LIBRARY})
message ("boost filesystem is" ${Boost_FILESYSTEM_LIBRARY})
message ("boost thread is" ${Boost_THREAD_LIBRARY})

set (AttributeNoninline "__attribute__ ((noinline))")
add_definitions (-DBOOST_NOINLINE=${AttributeNoninline})

message (${Boost_LIBRARIES})
target_link_libraries(testfem ${Boost_LIBRARIES})
