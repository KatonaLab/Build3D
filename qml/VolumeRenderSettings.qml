import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderSettings {
    id: root

    property color clearColor
    property alias camera: sceneCameraSelector.camera
    property size renderSize
    readonly property Texture2D backfaceMap: backfaceTexture

    renderPolicy: RenderSettings.OnDemand

    activeFrameGraph: Viewport {
        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

        RenderSurfaceSelector {
            CameraSelector {
                id: sceneCameraSelector
            
                RenderPassFilter {
                    matchAny: [FilterKey {name: "renderStyle"; value: "backface"}]

                    RenderTargetSelector {
                        target: RenderTarget {
                            attachments: [ RenderTargetOutput {
                                objectName: "backfaceMapTargetOutput"
                                attachmentPoint: RenderTargetOutput.Color0
                                texture: Texture2D {
                                    id: backfaceTexture
                                    width: renderSize.width
                                    height: renderSize.height
                                    format: Texture.RGBA32F
                                    generateMipMaps: false
                                    magnificationFilter: Texture.Linear
                                    minificationFilter: Texture.Linear
                                    wrapMode {
                                        x: WrapMode.ClampToEdge
                                        y: WrapMode.ClampToEdge
                                    }
                                } // texture: Texture2D
                            }] // attachments: RenderTargetOutput
                        } // RenderTargetSelector

                        ClearBuffers {
                            clearColor: Qt.rgba(0.0, 0.0, 0.0, 0.0)
                            buffers: ClearBuffers.ColorDepthBuffer
                            // clearDepthValue: 0.0
                        }
                    }
                }

                RenderPassFilter {
                    matchAny: [FilterKey {name: "renderStyle"; value: "forward"}]

                    ClearBuffers {
                        clearColor: root.clearColor
                        buffers: ClearBuffers.ColorDepthBuffer
                    }
                }

            } // sceneCameraSelector
        }
    }
}