add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)

if (BUILD_SMESH)
    add_definitions(-DHAVE_SMESH -DHAVE_MEFISTO)
endif(BUILD_SMESH)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${COIN3D_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    ${Python3_INCLUDE_DIRS}
    ${XercesC_INCLUDE_DIRS}
)

link_directories(${OCC_LIBRARY_DIR})
link_directories(${SMESH_LIB_PATH})

set(MeshPartGui_LIBS
    MeshPart
    PartGui
    MeshGui
    FreeCADGui
)

set(MeshPartGui_MOC_HDRS
    CurveOnMesh.h
    TaskCurveOnMesh.h
    Tessellation.h
)
fc_wrap_cpp(MeshPartGui_MOC_SRCS ${MeshPartGui_MOC_HDRS})
SOURCE_GROUP("Moc" FILES ${MeshPartGui_MOC_SRCS})

set(MeshPartGui_UIC_SRCS
    TaskCurveOnMesh.ui
    Tessellation.ui
)

    qt5_add_resources(MeshPartGui_QRC_SRCS Resources/MeshPart.qrc)
    qt5_wrap_ui(MeshPartGui_UIC_HDRS ${MeshPartGui_UIC_SRCS})

SET(MeshPartGui_SRCS
    ${MeshPartGui_QRC_SRCS}
    ${MeshPartGui_UIC_HDRS}
    AppMeshPartGui.cpp
    Command.cpp
    CurveOnMesh.cpp
    CurveOnMesh.h
    Resources/MeshPart.qrc
    TaskCurveOnMesh.ui
    TaskCurveOnMesh.cpp
    TaskCurveOnMesh.h
    Tessellation.ui
    Tessellation.cpp
    Tessellation.h
    Workbench.cpp
    Workbench.h
)

set(MeshPartGui_Scripts
    ../InitGui.py
)

set(FLATMESH_PY_SRCS
    MeshFlatteningCommand.py
)

add_library(MeshPartGui SHARED ${MeshPartGui_SRCS} ${MeshPartGui_Scripts} ${FLATMESH_PY_SRCS})
target_link_libraries(MeshPartGui ${MeshPartGui_LIBS})


SET_BIN_DIR(MeshPartGui MeshPartGui /Mod/MeshPart)
SET_PYTHON_PREFIX_SUFFIX(MeshPartGui)

fc_target_copy_resource_flat(MeshPartGui
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/Mod/MeshPart
    ${MeshPartGui_Scripts}
)

fc_copy_sources(MeshPartGui
   "${CMAKE_BINARY_DIR}/Mod/MeshPart"
    ${FLATMESH_PY_SRCS}
)

INSTALL(TARGETS MeshPartGui DESTINATION ${CMAKE_INSTALL_LIBDIR})
INSTALL(FILES ${FLATMESH_PY_SRCS} DESTINATION Mod/MeshPart)
