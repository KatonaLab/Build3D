import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

// import foo.bar 1.0

Entity {
    id: sceneRoot

    CuboidMesh {
        id: boxMesh
        xExtent: 5
        yExtent: 5
        zExtent: 5
    }

    // PhongMaterial {
        // id: material
    // }

    SimpleMaterial {
        id: simpleMaterial
        // objectName: "objVol"
    }

    // VolumeMaterial {
        // id: volumeMaterial
        // objectName: "objVol"
    // }

    Transform {
        id: boxTransform
        property real userAngle: 0.0
        matrix: {
            var m = Qt.matrix4x4();
            m.rotate(userAngle, Qt.vector3d(1, 1, 0));
            return m;
        }
    }

    Entity {
        id: boxEntity
        components: [ boxMesh, simpleMaterial, boxTransform ]
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -10.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController {
        camera: camera
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0, 0.5, 1, 1)
                camera: camera
            }
        }
        ,
        InputSettings { }
    ]

    SphereMesh {
        id: sphereMesh
        radius: 3
    }

    // Entity {
        // id: sphereEntity
        // components: [ sphereMesh, material ]
    // }
}