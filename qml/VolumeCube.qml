import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

import koki.katonalab.a3dc 1.0

// Component {

    Entity {

        id: root
        // property alias data: material.data
        property VolumetricData data: undefined

        VolumetricMaterial {
            id: material
            data: root.data
        }

        CuboidMesh {
            id: boxMesh
            xExtent: 1
            yExtent: 1
            zExtent: 1
        }

        components: [ boxMesh, simpleMaterial ]
    }
// }