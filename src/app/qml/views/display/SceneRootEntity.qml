import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

import "../../stores"

Entity {

    components: [
        SceneRenderSettings {
            id: renderSettings
            camera: camera
            clearColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            renderSize: Qt.size(width, height)
            sceneLayer: sceneLayer
            screenQuadLayer: screenQuadLayer
        },
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


    // TODO: find a way to do the rendering like this:
    // 
    // for 3d_objects:
    //   do_backface_render -> backfacebuffer
    //   do_frontface_render(backfacebuffer) -> accum buffer
    // render_accum_buffer
    // 
    // so we need a per object volume rendering and then merging the results

    NodeInstantiator {
        model: MainStore.sceneStore.model
        delegate: VolumeEntity {
            uid: model.uid

            width: model.size.x
            height: model.size.y
            depth: model.size.z
            volumeTexture: model.texture

            backFaceMap: renderSettings.backFaceMap
            // TODO: count only the visible channels
            // accumDivisor: 1.0 / MainStore.moduleStore.model.count
            accumDivisor: 1.0
            layer: sceneLayer

            volumeColor: "red"
            visible: true

            lutLowCut: 0
            lutHighCut: 1
            Component.onCompleted: {
                // lutDataMax = model.data.dataLimits.y;
                lutDataMax = 10.;
            }

            // volumeColor: model.nodeViewParams.color
            // visible: model.nodeViewParams.visible

            // lutLowCut: model.nodeViewParams.lowCut
            // lutHighCut: model.nodeViewParams.highCut
            // Component.onCompleted: {
            //     //lutDataMax = model.data.dataLimits.y;
            //     lutDataMax = 1000.;
            // }
        }
    }

    Layer {
        id: screenQuadLayer
    }

    Layer {
        id: sceneLayer
    }

    ScreenQuadEntity {
        layer: screenQuadLayer
        frontFaceAccumulatorMap: renderSettings.frontFaceAccumulatorMap
    }
}
