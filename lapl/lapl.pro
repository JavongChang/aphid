mac:LIBS += -leasymodel
INCLUDEPATH += ../shared 
win32:INCLUDEPATH += D:/ofl/shared D:/usr/libxml2x64/include D:/usr/eigen2
mac:INCLUDEPATH += /Users/jianzhang/Library/eigen2
win32:LIBS +=  -LD:/usr/local/lib64 -leasymodel -LD:/usr/libxml2x64/lib -llibxml2
HEADERS       = glwidget.h \
                window.h \
                ../shared/Vector3F.h \
                ../shared/Matrix44F.h \
                ../shared/Matrix33F.h \
                ../shared/BaseCamera.h \
                ../shared/Base3DView.h \
                ../shared/TypedEntity.h \
                ../shared/Geometry.h \
                ../shared/BaseMesh.h \
                ../shared/BoundingBox.h \
                ../shared/TriangleMesh.h \
                ../shared/BaseDrawer.h \
				../shared/KdTreeDrawer.h \
                ../shared/GeoElement.h \
                ../shared/Vertex.h \
                ../shared/Edge.h \
                ../shared/Facet.h \
                ../shared/BaseDeformer.h \
                ../shared/KdTreeNode.h \
                ../shared/KdTree.h \
                ../shared/Primitive.h \
                ../shared/BuildKdTreeContext.h \
                ../shared/KdTreeNodeArray.h \
                ../shared/PrimitiveArray.h \
                ../shared/IndexArray.h \
                ../shared/BaseArray.h \
                ../shared/ClassificationStorage.h \
                ../shared/KdTreeBuilder.h \
                ../shared/SplitEvent.h \
                ../shared/MinMaxBins.h \
                ../shared/BuildKdTreeStream.h \
                ../shared/IndexList.h \
                ../shared/BoundingBoxList.h \
                MeshLaplacian.h \
                VertexAdjacency.h \
                LaplaceDeformer.h
                
SOURCES       = glwidget.cpp \
                main.cpp \
                window.cpp \
                ../shared/Vector3F.cpp \
                ../shared/Matrix44F.cpp \
                ../shared/Matrix33F.cpp \
                ../shared/BaseCamera.cpp \
                ../shared/Base3DView.cpp \
                ../shared/TypedEntity.cpp \
                ../shared/Geometry.cpp \
                ../shared/BoundingBox.cpp \
                ../shared/BaseMesh.cpp \
                ../shared/TriangleMesh.cpp \
                ../shared/BaseDrawer.cpp \
				../shared/KdTreeDrawer.cpp \
                ../shared/GeoElement.cpp \
                ../shared/Vertex.cpp \
                ../shared/Edge.cpp \
                ../shared/Facet.cpp \
                ../shared/BaseDeformer.cpp \
                ../shared/KdTreeNode.cpp \
                ../shared/KdTree.cpp \
                ../shared/Primitive.cpp \
                ../shared/BuildKdTreeContext.cpp \
                ../shared/KdTreeNodeArray.cpp \
                ../shared/PrimitiveArray.cpp \
                ../shared/IndexArray.cpp \
                ../shared/BaseArray.cpp \
                ../shared/ClassificationStorage.cpp \
                ../shared/KdTreeBuilder.cpp \
                ../shared/SplitEvent.cpp \
                ../shared/MinMaxBins.cpp \
                ../shared/BuildKdTreeStream.cpp \
                ../shared/IndexList.cpp \
                ../shared/BoundingBoxList.cpp \
                MeshLaplacian.cpp \
                VertexAdjacency.cpp \
                LaplaceDeformer.cpp
win32 {
    HEADERS += ../shared/gExtension.h
    SOURCES += ../shared/gExtension.cpp
    INCLUDEPATH += D:/usr/local/include
    QMAKE_LIBDIR += D:/usr/local/lib64
CONFIG += console
}
macx {
    INCLUDEPATH += ../../Library/boost_1_44_0
        LIBS += -lboost_date_time\
            -lboost_thread
}
QT           += opengl
win32:CONFIG += console
win32:    DEFINES += NOMINMAX
#mac:CONFIG -= app_bundle
