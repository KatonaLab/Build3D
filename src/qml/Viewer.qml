import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

// import foo.bar 1.0

Entity {
    id: sceneRoot

    property alias thresholding: simpleMaterial.thresholding
    property alias threshold: simpleMaterial.threshold

    CuboidMesh {
        id: boxMesh
        xExtent: 1
        yExtent: 1
        zExtent: 1
    }

    VolumetricMaterial {
        id: simpleMaterial
        // objectName: "objVol"
    }

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
        // projectionType: CameraLens.OrthographicProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d( 0.0, 0.0, -2.0 )
        upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
        viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
    }

    OrbitCameraController {
        camera: camera
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0.0, 0.0, 0.0, 1)
                camera: camera
            }
        }
        ,
        InputSettings { }
    ]
}