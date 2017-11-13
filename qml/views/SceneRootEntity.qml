import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

import "../stores"

Entity {

    components: [
        SceneRenderSettings {
            id: renderSettings
            clearColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            camera: camera
            renderSize: Qt.size(width, height)
        },
        // RenderSettings {
        //     activeFrameGraph: ForwardRenderer {
        //         camera: camera
        //         // clearColor: "transparent"
        //         clearColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        //     }
        // },
        InputSettings {}
    ]

    Camera {
        id: camera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 16/9
        nearPlane : 0.1
        farPlane : 100.0
        position: Qt.vector3d(0.0, 0.0, -2.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    OrbitCameraController {
        camera: camera
        lookSpeed: -180 * 2
    }

    NodeInstantiator {
        model: MainStore.sceneStore.model
        delegate: VolumeEntity {
            uid: uid
            width: model.width
            height: model.height
            depth: model.depth
            volumeData: model.data
            backfaceMap: renderSettings.backfaceMap
            volumeColor: model.viewParameters.color

            Component.onCompleted: {
                var y = model.data.dataLimits.y;
                lutParameters = Qt.binding(function() { 
                    return Qt.vector4d(0, y, model.viewParameters.lowCut, 
                        model.viewParameters.highCut); 
                });

                volumeColor = Qt.binding(function() { 
                    var col = model.viewParameters.color;
                    col.a = model.viewParameters.visible ? 1. : 0.;
                    return col;
                });
            }
        }
    }
}