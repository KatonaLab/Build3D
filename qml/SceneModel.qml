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
        var object = component.createObject(root, 
            {"color" : Qt.rgba(Math.random(), Math.random(), Math.random(), 1)}
            );
        d.cubeList.push(object);
        return object;
    }

    // property alias thresholding: simpleMaterial.thresholding
    // property alias threshold: simpleMaterial.threshold

    // Component {
    //     id: myModel
        
    //     // property alias data: material.data

    //     Entity{
    //         VolumetricMaterial {
    //             id: material
    //         }

    //         CuboidMesh {
    //             id: boxMesh
    //             xExtent: 1
    //             yExtent: 1
    //             zExtent: 1
    //         }

    //         components: [ boxMesh, simpleMaterial ]
    //     }
    // }

    // function addModel(volumeData)
    // {
        // var component = Qt.createComponent("VolumeCube.qml");
        // console.log(component.errorString());

        // var obj = component.createObject(root, {"data": volumeData});

        // if (obj == null) {
        //     console.log("Error creating object");
        // }
    // }

    // CuboidMesh {
    //     id: boxMesh
    //     xExtent: 1
    //     yExtent: 1
    //     zExtent: 1
    // }

    // VolumetricMaterial {
    //     id: simpleMaterial
    //     // objectName: "objVol"
    // }

    // DiffuseMapMaterial {
        // id: simpleMaterial
        // ambient: "#225588"
    // }

    // Transform {
    //     id: boxTransform
    //     property real userAngle: 0.0
    //     matrix: {
    //         var m = Qt.matrix4x4();
    //         m.rotate(userAngle, Qt.vector3d(1, 1, 0));
    //         return m;
    //     }
    // }

    // Entity {
    //     id: boxEntity
    //     components: [ boxMesh, simpleMaterial, boxTransform ]
    // }

    // Entity {
        // id: boxEntity
        // components: [ boxMesh, simpleMaterial ]
    // }

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
        lookSpeed: -180
    }

    components: [
        RenderSettings {
            activeFrameGraph: ForwardRenderer {
                clearColor: Qt.rgba(0.0, 0.2, 0.2, 1)
                camera: camera
            }
        },
        InputSettings { }
    ]
}