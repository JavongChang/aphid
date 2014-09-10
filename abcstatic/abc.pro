TEMPLATE = lib
TARGET = alembic
CONFIG += staticlib thread release
CONFIG -= qt
win32 {
ABC_SRC = D:/usr/Alembic/lib/Alembic
HDF5_ROOT = D:/usr/hdf5
BOOST_ROOT = D:/usr/boost_1_51_0
}
unix {
ABC_SRC = /home/OF3D/zhangjian/Documents/alembic/lib/Alembic
HDF5_ROOT = /home/OF3D/zhangjian/Documents/hdf5-1.8.9/hdf5
BOOST_ROOT = /home/OF3D/zhangjian/Documents/boost_1_51_0
}

INCLUDEPATH += $$HDF5_ROOT/include \
                $$MAYA_ROOT/include \
                $$BOOST_ROOT 

win32 {
INCLUDEPATH += D:/usr/alembic/lib \
		D:/usr/openEXR/include \
                D:/usr/local/include/boost/tr1/tr1
}

unix {
INCLUDEPATH += /usr/autodesk/maya/devkit/Alembic/include/AlembicPrivate/OpenEXR \
		/home/OF3D/zhangjian/Documents/alembic/lib
}
                
QMAKE_LIBDIR += $$HDF5_ROOT/lib

HEADERS       = $$ABC_SRC/Abc/All.h \
                $$ABC_SRC/Abc/ArchiveInfo.h \
                $$ABC_SRC/Abc/Argument.h \
                $$ABC_SRC/Abc/Base.h \
                $$ABC_SRC/Abc/ErrorHandler.h \
                $$ABC_SRC/Abc/Foundation.h \
                $$ABC_SRC/Abc/IArchive.h \
                $$ABC_SRC/Abc/IArrayProperty.h \
                $$ABC_SRC/Abc/IBaseProperty.h \
                $$ABC_SRC/Abc/ICompoundProperty.h \
                $$ABC_SRC/Abc/IObject.h \
                $$ABC_SRC/Abc/ISampleSelector.h \
                $$ABC_SRC/Abc/IScalarProperty.h \
                $$ABC_SRC/Abc/ISchema.h \
                $$ABC_SRC/Abc/ISchemaObject.h \
                $$ABC_SRC/Abc/ITypedArrayProperty.h \
                $$ABC_SRC/Abc/ITypedScalarProperty.h \
                $$ABC_SRC/Abc/OArchive.h \
                $$ABC_SRC/Abc/OArrayProperty.h \
                $$ABC_SRC/Abc/OBaseProperty.h \
                $$ABC_SRC/Abc/OCompoundProperty.h \
                $$ABC_SRC/Abc/OObject.h \
                $$ABC_SRC/Abc/OScalarProperty.h \
                $$ABC_SRC/Abc/OSchema.h \
                $$ABC_SRC/Abc/OSchemaObject.h \
                $$ABC_SRC/Abc/OTypedArrayProperty.h \
                $$ABC_SRC/Abc/OTypedScalarProperty.h \
                $$ABC_SRC/Abc/Reference.h \
                $$ABC_SRC/Abc/SourceName.h \
                $$ABC_SRC/Abc/TypedArraySample.h \
                $$ABC_SRC/Abc/TypedPropertyTraits.h \
                $$ABC_SRC/AbcCollection/All.h \
                $$ABC_SRC/AbcCollection/SchemaInfoDeclarations.h \
                $$ABC_SRC/AbcCollection/OCollections.h \
                $$ABC_SRC/AbcCollection/ICollections.h \
                $$ABC_SRC/AbcCoreAbstract/All.h \
                $$ABC_SRC/AbcCoreAbstract/ArchiveReader.h \
                $$ABC_SRC/AbcCoreAbstract/ArchiveWriter.h  \
                $$ABC_SRC/AbcCoreAbstract/ArrayPropertyReader.h \
                $$ABC_SRC/AbcCoreAbstract/ArrayPropertyWriter.h \
                $$ABC_SRC/AbcCoreAbstract/ArraySample.h \
                $$ABC_SRC/AbcCoreAbstract/ArraySampleKey.h \
                $$ABC_SRC/AbcCoreAbstract/BasePropertyReader.h \
                $$ABC_SRC/AbcCoreAbstract/BasePropertyWriter.h \
                $$ABC_SRC/AbcCoreAbstract/CompoundPropertyWriter.h \
                $$ABC_SRC/AbcCoreAbstract/CompoundPropertyReader.h \
                $$ABC_SRC/AbcCoreAbstract/DataType.h \
                $$ABC_SRC/AbcCoreAbstract/ForwardDeclarations.h \
                $$ABC_SRC/AbcCoreAbstract/Foundation.h \
                $$ABC_SRC/AbcCoreAbstract/MetaData.h \
                $$ABC_SRC/AbcCoreAbstract/ObjectHeader.h \
                $$ABC_SRC/AbcCoreAbstract/ObjectReader.h  \
                $$ABC_SRC/AbcCoreAbstract/ObjectWriter.h  \
                $$ABC_SRC/AbcCoreAbstract/PropertyHeader.h \
                $$ABC_SRC/AbcCoreAbstract/ReadArraySampleCache.h \
                $$ABC_SRC/AbcCoreAbstract/ScalarPropertyReader.h \
                $$ABC_SRC/AbcCoreAbstract/ScalarPropertyWriter.h \
                $$ABC_SRC/AbcCoreAbstract/ScalarSample.h \
                $$ABC_SRC/AbcCoreAbstract/TimeSampling.h \
                $$ABC_SRC/AbcCoreAbstract/TimeSamplingType.h \ 
                $$ABC_SRC/AbcCoreFactory/All.h \
                $$ABC_SRC/AbcCoreFactory/IFactory.h \
                $$ABC_SRC/AbcCoreHDF5/All.h \
                $$ABC_SRC/AbcCoreHDF5/AprImpl.h \
                $$ABC_SRC/AbcCoreHDF5/ApwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/ArImpl.h \
                $$ABC_SRC/AbcCoreHDF5/AwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/CacheImpl.h \
                $$ABC_SRC/AbcCoreHDF5/CprData.h \
                $$ABC_SRC/AbcCoreHDF5/CprImpl.h \
                $$ABC_SRC/AbcCoreHDF5/CpwData.h \
                $$ABC_SRC/AbcCoreHDF5/CpwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/DataTypeRegistry.h \
                $$ABC_SRC/AbcCoreHDF5/Foundation.h \
                $$ABC_SRC/AbcCoreHDF5/HDF5Hierarchy.h \
                $$ABC_SRC/AbcCoreHDF5/HDF5HierarchyReader.h \
                $$ABC_SRC/AbcCoreHDF5/HDF5HierarchyWriter.h \
                $$ABC_SRC/AbcCoreHDF5/HDF5Util.h \
                $$ABC_SRC/AbcCoreHDF5/OrData.h \
                $$ABC_SRC/AbcCoreHDF5/OrImpl.h \
                $$ABC_SRC/AbcCoreHDF5/OwData.h \
                $$ABC_SRC/AbcCoreHDF5/OwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/ReadUtil.h \
                $$ABC_SRC/AbcCoreHDF5/ReadWrite.h \
                $$ABC_SRC/AbcCoreHDF5/SimplePrImpl.h \
                $$ABC_SRC/AbcCoreHDF5/SimplePwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/SprImpl.h \
                $$ABC_SRC/AbcCoreHDF5/SpwImpl.h \
                $$ABC_SRC/AbcCoreHDF5/StringReadUtil.h \
                $$ABC_SRC/AbcCoreHDF5/StringWriteUtil.h \
                $$ABC_SRC/AbcCoreHDF5/WriteUtil.h \
                $$ABC_SRC/AbcCoreHDF5/WrittenArraySampleMap.h \
                $$ABC_SRC/Util/Digest.h \
                $$ABC_SRC/Util/Dimensions.h \
                $$ABC_SRC/Util/Exception.h \
                $$ABC_SRC/Util/Foundation.h \
                $$ABC_SRC/Util/Murmur3.h \
                $$ABC_SRC/Util/OperatorBool.h \
                $$ABC_SRC/Util/PlainOldDataType.h \
                $$ABC_SRC/Util/TokenMap.h \
                $$ABC_SRC/Util/All.h \
                $$ABC_SRC/AbcGeom/All.h \
                $$ABC_SRC/AbcGeom/Foundation.h \
                $$ABC_SRC/AbcGeom/ArchiveBounds.h \
                $$ABC_SRC/AbcGeom/IGeomBase.h \
                $$ABC_SRC/AbcGeom/OGeomBase.h \
                $$ABC_SRC/AbcGeom/GeometryScope.h \
                $$ABC_SRC/AbcGeom/SchemaInfoDeclarations.h \
                $$ABC_SRC/AbcGeom/OLight.h \
                $$ABC_SRC/AbcGeom/ILight.h \
                $$ABC_SRC/AbcGeom/FilmBackXformOp.h \
                $$ABC_SRC/AbcGeom/CameraSample.h \
                $$ABC_SRC/AbcGeom/ICamera.h \
                $$ABC_SRC/AbcGeom/OCamera.h \
                $$ABC_SRC/AbcGeom/Basis.h \
                $$ABC_SRC/AbcGeom/CurveType.h \
                $$ABC_SRC/AbcGeom/ICurves.h \
                $$ABC_SRC/AbcGeom/OCurves.h \
                $$ABC_SRC/AbcGeom/FaceSetExclusivity.h \
                $$ABC_SRC/AbcGeom/OFaceSet.h \
                $$ABC_SRC/AbcGeom/IFaceSet.h \
                $$ABC_SRC/AbcGeom/ONuPatch.h \
                $$ABC_SRC/AbcGeom/INuPatch.h \
                $$ABC_SRC/AbcGeom/OGeomParam.h \
                $$ABC_SRC/AbcGeom/IGeomParam.h \
                $$ABC_SRC/AbcGeom/OPoints.h \
                $$ABC_SRC/AbcGeom/IPoints.h \
                $$ABC_SRC/AbcGeom/OPolyMesh.h \
                $$ABC_SRC/AbcGeom/IPolyMesh.h \
                $$ABC_SRC/AbcGeom/OSubD.h \
                $$ABC_SRC/AbcGeom/ISubD.h \
                $$ABC_SRC/AbcGeom/Visibility.h \
                $$ABC_SRC/AbcGeom/XformOp.h \
                $$ABC_SRC/AbcGeom/XformSample.h \
                $$ABC_SRC/AbcGeom/IXform.h \
                $$ABC_SRC/AbcGeom/OXform.h \
                $$ABC_SRC/AbcMaterial/SchemaInfoDeclarations.h \
                $$ABC_SRC/AbcMaterial/OMaterial.h \
                $$ABC_SRC/AbcMaterial/IMaterial.h \
                $$ABC_SRC/AbcMaterial/MaterialFlatten.h \
                $$ABC_SRC/AbcMaterial/MaterialAssignment.h 
                
SOURCES       = $$ABC_SRC/Abc/ArchiveInfo.cpp \
                $$ABC_SRC/Abc/ErrorHandler.cpp \
                $$ABC_SRC/Abc/IArchive.cpp \
                $$ABC_SRC/Abc/IArrayProperty.cpp \
                $$ABC_SRC/Abc/ICompoundProperty.cpp \
                $$ABC_SRC/Abc/IObject.cpp \
                $$ABC_SRC/Abc/ISampleSelector.cpp \
                $$ABC_SRC/Abc/IScalarProperty.cpp \
                $$ABC_SRC/Abc/OArchive.cpp \
                $$ABC_SRC/Abc/OArrayProperty.cpp \
                $$ABC_SRC/Abc/OCompoundProperty.cpp \
                $$ABC_SRC/Abc/OObject.cpp \
                $$ABC_SRC/Abc/OScalarProperty.cpp \
                $$ABC_SRC/Abc/Reference.cpp \
                $$ABC_SRC/Abc/SourceName.cpp \
                $$ABC_SRC/AbcCollection/ICollections.cpp \
                $$ABC_SRC/AbcCollection/OCollections.cpp \
                $$ABC_SRC/AbcCoreAbstract/Foundation.cpp \
                $$ABC_SRC/AbcCoreAbstract/TimeSampling.cpp \
                $$ABC_SRC/AbcCoreAbstract/TimeSamplingType.cpp \
                $$ABC_SRC/AbcCoreAbstract/ArraySample.cpp \
                $$ABC_SRC/AbcCoreAbstract/ReadArraySampleCache.cpp \
                $$ABC_SRC/AbcCoreAbstract/ScalarSample.cpp \
                $$ABC_SRC/AbcCoreAbstract/BasePropertyWriter.cpp \
                $$ABC_SRC/AbcCoreAbstract/ScalarPropertyWriter.cpp \
                $$ABC_SRC/AbcCoreAbstract/ArrayPropertyWriter.cpp \
                $$ABC_SRC/AbcCoreAbstract/CompoundPropertyWriter.cpp \
                $$ABC_SRC/AbcCoreAbstract/ObjectWriter.cpp  \
                $$ABC_SRC/AbcCoreAbstract/ArchiveWriter.cpp  \
                $$ABC_SRC/AbcCoreAbstract/BasePropertyReader.cpp \
                $$ABC_SRC/AbcCoreAbstract/ScalarPropertyReader.cpp \
                $$ABC_SRC/AbcCoreAbstract/ArrayPropertyReader.cpp \
                $$ABC_SRC/AbcCoreAbstract/CompoundPropertyReader.cpp \
                $$ABC_SRC/AbcCoreAbstract/ObjectReader.cpp  \
                $$ABC_SRC/AbcCoreAbstract/ArchiveReader.cpp  \
                $$ABC_SRC/AbcCoreFactory/IFactory.cpp \
                $$ABC_SRC/AbcCoreHDF5/AprImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/ApwImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/ArImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/AwImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/CacheImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/CprData.cpp \
                $$ABC_SRC/AbcCoreHDF5/CprImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/CpwData.cpp \
                $$ABC_SRC/AbcCoreHDF5/CpwImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/DataTypeRegistry.cpp \
                $$ABC_SRC/AbcCoreHDF5/HDF5Hierarchy.cpp \
                $$ABC_SRC/AbcCoreHDF5/HDF5HierarchyReader.cpp \
                $$ABC_SRC/AbcCoreHDF5/HDF5HierarchyWriter.cpp \
                $$ABC_SRC/AbcCoreHDF5/HDF5Util.cpp \
                $$ABC_SRC/AbcCoreHDF5/OrData.cpp \
                $$ABC_SRC/AbcCoreHDF5/OrImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/OwData.cpp \
                $$ABC_SRC/AbcCoreHDF5/OwImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/ReadUtil.cpp \
                $$ABC_SRC/AbcCoreHDF5/ReadWrite.cpp \
                $$ABC_SRC/AbcCoreHDF5/SprImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/SpwImpl.cpp \
                $$ABC_SRC/AbcCoreHDF5/StringReadUtil.cpp \
                $$ABC_SRC/AbcCoreHDF5/StringWriteUtil.cpp \
                $$ABC_SRC/AbcCoreHDF5/WriteUtil.cpp \
                $$ABC_SRC/Util/Murmur3.cpp \
     		$$ABC_SRC/Util/Naming.cpp \
		$$ABC_SRC/Util/TokenMap.cpp \
     		$$ABC_SRC/Util/SpookyV2.cpp \
                $$ABC_SRC/AbcGeom/ArchiveBounds.cpp \
                $$ABC_SRC/AbcGeom/GeometryScope.cpp \
                $$ABC_SRC/AbcGeom/FilmBackXformOp.cpp \
                $$ABC_SRC/AbcGeom/CameraSample.cpp \
                $$ABC_SRC/AbcGeom/ICamera.cpp \
                $$ABC_SRC/AbcGeom/OCamera.cpp \
                $$ABC_SRC/AbcGeom/Basis.cpp \
                $$ABC_SRC/AbcGeom/ICurves.cpp \
                $$ABC_SRC/AbcGeom/OCurves.cpp \
                $$ABC_SRC/AbcGeom/OFaceSet.cpp \
                $$ABC_SRC/AbcGeom/IFaceSet.cpp \
                $$ABC_SRC/AbcGeom/OLight.cpp \
                $$ABC_SRC/AbcGeom/ILight.cpp \
                $$ABC_SRC/AbcGeom/ONuPatch.cpp \
                $$ABC_SRC/AbcGeom/INuPatch.cpp \
                $$ABC_SRC/AbcGeom/OPoints.cpp \
                $$ABC_SRC/AbcGeom/IPoints.cpp \
                $$ABC_SRC/AbcGeom/OPolyMesh.cpp \
                $$ABC_SRC/AbcGeom/IPolyMesh.cpp \
                $$ABC_SRC/AbcGeom/OSubD.cpp \
                $$ABC_SRC/AbcGeom/ISubD.cpp \
                $$ABC_SRC/AbcGeom/Visibility.cpp \
                $$ABC_SRC/AbcGeom/XformOp.cpp \
                $$ABC_SRC/AbcGeom/XformSample.cpp \
                $$ABC_SRC/AbcGeom/IXform.cpp \
                $$ABC_SRC/AbcGeom/OXform.cpp \
                $$ABC_SRC/AbcMaterial/OMaterial.cpp \
                $$ABC_SRC/AbcMaterial/IMaterial.cpp \
                $$ABC_SRC/AbcMaterial/MaterialFlatten.cpp \
                $$ABC_SRC/AbcMaterial/MaterialAssignment.cpp \
                $$ABC_SRC/AbcMaterial/InternalUtil.cpp
                
win32 {
DEFINES += OPENEXR_DLL NDEBUG NOMINMAX
}



