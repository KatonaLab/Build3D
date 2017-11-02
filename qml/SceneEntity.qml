import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

import Qt3D.Input 2.0
import Qt3D.Logic 2.0

Entity {
    id: root

    property alias volumeCube: cube

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        // aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 100.0
        position: Qt.vector3d(0.0, 0.0, -2.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
    // FirstPersonCameraController {
        camera: camera
        lookSpeed: -180 * 2
    }
    
    // Entity {
    //     components: [
    //     FrameAction {
    //         onTriggered: {
    //             camera.translate(Qt.vector3d(0.001,0, 0), Camera.DontTranslateViewCenter);
    //             // var up = Qt.vector3d(0, 1, 0);
    //             // camera.pan(Qt.vector3d(0, 0.00001, 0), up)
    //             //root.camera.panAboutViewCenter(root.lookSpeed * d.dx * dt, d.firstPersonUp)
    //         }
    //     }]
    // }

    VolumeCube {
        id: cube
        backfaceMap: renderSettings.backfaceMap
        volumeParameter0: VolumeParameters {}
        volumeParameter1: VolumeParameters {}
        volumeParameter2: VolumeParameters {}
        volumeParameter3: VolumeParameters {}
    }

    components: [
        VolumeRenderSettings {
            id: renderSettings
            clearColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            camera: camera
            renderSize: Qt.size(width, height)
        },
        InputSettings { }
    ]
}