import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderSettings {
    id: root

    property color clearColor
    property alias camera: sceneCameraSelector.camera

    activeFrameGraph: Viewport {
        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

        RenderSurfaceSelector {
            RenderPassFilter {
                matchAny: [FilterKey {name: "renderStyle"; value: "forward"}]

                ClearBuffers {
                    clearColor: root.clearColor
                    buffers: ClearBuffers.ColorDepthBuffer
                    clearDepthValue: 0.0

                    CameraSelector {
                        id: sceneCameraSelector
                    }
                }
            }
        }
    }
}