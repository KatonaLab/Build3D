import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

Entity {
    id: root
    property alias color: material.diffuse

    PhongMaterial {
        id: material
    }

    CuboidMesh {
        id: mesh
        xExtent: Math.random()
        yExtent: Math.random()
        zExtent: Math.random()
    }

    components: [mesh, material]
}