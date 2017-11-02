import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

Entity {
    id: root

    property vector3d size
    property alias backfaceMap: material.backfaceMap
    property alias volumeParameter0: material.volumeParameter0
    property alias volumeParameter1: material.volumeParameter1
    property alias volumeParameter2: material.volumeParameter2
    property alias volumeParameter3: material.volumeParameter3

    VolumeMaterial {
        id: material
        vertexToTex3DCoordMatrix: Qt.matrix4x4(
            1/size.x, 0, 0, 0.5,
            0, 1/size.y, 0, 0.5,
            0, 0, 1/size.z, 0.5,
            0, 0, 0, 1)
    }
 
    GeometryRenderer {
        id: mesh
        instanceCount: 1 // we need one instance
        geometry: cubeGeo
    }

    CuboidGeometry {
        id: cubeGeo
        xExtent: size.x
        yExtent: size.y
        zExtent: size.z
    }

    components: [mesh, material]
}