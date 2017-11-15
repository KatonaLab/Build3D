import Qt3D.Core 2.0
import Qt3D.Render 2.0

RenderSettings {
    id: root

    property color clearColor
    property alias camera: sceneCameraSelector.camera
    property size renderSize
    property Layer sceneLayer
    property Layer screenQuadLayer

    readonly property Texture2D backFaceMap: backFaceTexture
    readonly property Texture2D frontFaceAccumulatorMap: frontFaceAccumulatorTexture

    renderPolicy: RenderSettings.OnDemand

    activeFrameGraph: Viewport {
        normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

        // TODO: is RenderSurfaceSelector neccessary?
        RenderSurfaceSelector {
            CameraSelector {
                id: sceneCameraSelector

                LayerFilter {
                    layers: [sceneLayer]
                    RenderPassFilter {
                        matchAny: [FilterKey {name: "pass"; value: "BackFace"}]

                        RenderTargetSelector {
                            target: RenderTarget {
                                attachments: [ RenderTargetOutput {
                                    // objectName: "backFaceMapTargetOutput"
                                    attachmentPoint: RenderTargetOutput.Color0
                                    texture: Texture2D {
                                        id: backFaceTexture
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
                            } // RenderTarget

                            ClearBuffers {
                                clearColor: Qt.rgba(0.0, 0.0, 0.0, 0.0)
                                buffers: ClearBuffers.ColorDepthBuffer
                            }
                        } // RenderTargetSelector
                    } // RenderPassFilter

                    RenderPassFilter {
                        matchAny: [FilterKey {name: "pass"; value: "FrontFaceAccumulate"}]

                        RenderTargetSelector {
                            target: RenderTarget {
                                attachments: [ RenderTargetOutput {
                                    // objectName: "frontFaceMapTargetOutput"
                                    attachmentPoint: RenderTargetOutput.Color0
                                    texture: Texture2D {
                                        id: frontFaceAccumulatorTexture
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
                                clearColor: root.clearColor
                                buffers: ClearBuffers.ColorDepthBuffer
                            }
                        } // RenderTargetSelector
                    } // RenderPassFilter
                } // LayerFilter
                
                LayerFilter {
                    layers: [screenQuadLayer]
                    RenderPassFilter {
                        matchAny: [FilterKey {name: "pass"; value: "Final"}]
                        ClearBuffers {
                            clearColor: root.clearColor
                            buffers: ClearBuffers.ColorDepthBuffer
                        }
                    }
                } // layerfilter
            } // sceneCameraSelector
        }
    }
}