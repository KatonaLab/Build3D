import QtQuick 2.0
import QtQuick.Scene3D 2.0
import Qt3D.Render 2.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Item {

    Rectangle {
        id: scene
        property bool colorChange: true
        anchors.fill: parent
        color: "White"

        transform: Rotation {
            id: sceneRotation
            axis.x: 1
            axis.y: 0
            axis.z: 0
            origin.x: scene.width / 2
            origin.y: scene.height / 2
        }

        Scene3D {
            id: scene3d
            anchors.fill: parent
            anchors.margins: 10
            focus: true
            aspects: ["input", "logic"]
            cameraAspectRatioMode: Scene3D.AutomaticAspectRatio

            Entity {
                id: sceneRoot

                CuboidMesh {
                    id: boxMesh
                    xExtent: 5
                    yExtent: 5
                    zExtent: 5
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
                    components: [ boxMesh, material, boxTransform ]
                }

                Camera {
                    id: camera
                    projectionType: CameraLens.PerspectiveProjection
                    fieldOfView: 45
                    aspectRatio: 16/9
                    nearPlane : 0.1
                    farPlane : 1000.0
                    position: Qt.vector3d( 0.0, 0.0, -20.0 )
                    upVector: Qt.vector3d( 0.0, 1.0, 0.0 )
                    viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                }

                components: [
                    RenderSettings {
                        activeFrameGraph: ForwardRenderer {
                            clearColor: Qt.rgba(0, 0.5, 1, 1)
                            camera: camera
                        }
                    }
                    // InputSettings { }
                ]

                PhongMaterial {
                    id: material
                }

                SphereMesh {
                    id: sphereMesh
                    radius: 3
                }

                Entity {
                    id: sphereEntity
                    components: [ sphereMesh, material ]
                }
            }
        }
    }
}