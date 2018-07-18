import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

import "../../stores"

Entity {
    id: root
    property size viewPortSize: Qt.size(1, 1);

    components: [
        SceneRenderSettings {
            id: renderSettings
            camera: sceneCamera
            clearColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
            renderSize: Qt.size(width, height)
            sceneLayer: sceneLayer
            screenQuadLayer: screenQuadLayer
        },
        InputSettings {}
    ]

    Camera {
        id: sceneCamera
        projectionType: CameraLens.PerspectiveProjection
        fieldOfView: 45
        aspectRatio: 3/4
        nearPlane : 0.1
        farPlane : 100.0
        position: Qt.vector3d(0.0, 0.0, -2.0)
        upVector: Qt.vector3d(0.0, 1.0, 0.0)
        viewCenter: Qt.vector3d(0.0, 0.0, 0.0)
    }

    TurnTableCameraController {
        camera: sceneCamera
        viewPortSize: root.viewPortSize
        rollBallRadius: viewPortSize.width * 0.4
        lookSpeed: 180
        linearSpeed: 1
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

            // note: JSValue to QColor conversion
            volumeColor: Qt.rgba(model.color.r, model.color.g, model.color.b, model.color.a);
            visible: model.visible
            labeled: model.labeled

            lutLowCut: model.lutLow
            lutHighCut: model.lutHigh
            Component.onCompleted: {
                lutDataMax = 1.;
            }
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