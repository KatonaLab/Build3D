import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0

Entity {
    id: root

    QtObject {
        id: d
        property var cubeList : []
    }

    function clearCubes()
    {
        for (var i = 0; i < d.cubeList.length; ++i) {
            d.cubeList[i].destroy();
        }
        d.cubeList = [];
    }

    function createCube(volumeData)
    {
        var component = Qt.createComponent("VolumeCube.qml");
        // TODO: set the ranges properly
        var object = component.createObject(root);
        d.cubeList.push(object);
        return object;
    }

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 1000.0
        position: Qt.vector3d(0.0, 0.0, -2.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
        camera: camera
        lookSpeed: -180
    }

    components: [
        VolumeRenderSettings {
            clearColor: Qt.rgba(0.0, 0.4, 0.6, 1)
            camera: camera
        },
        InputSettings { }
    ]
}