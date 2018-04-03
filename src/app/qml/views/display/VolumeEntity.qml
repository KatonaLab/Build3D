import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import koki.katonalab.a3dc 1.0

Entity {

    property int uid

    property real width
    property real height
    property real depth
    property VolumeTexture volumeTexture

    property color volumeColor
    property real accumDivisor
    property bool visible
    property real lutLowCut
    property real lutHighCut
    property real lutDataMax

    property vector4d lutParameters: Qt.vector4d(0, lutDataMax, lutLowCut, lutHighCut)

    property Layer layer
    property Texture2D backFaceMap    

    QtObject {
        id: d
        property matrix4x4 vertexToTex3DCoordMatrix: Qt.matrix4x4(
            1/width, 0, 0, 0.5,
            0, 1/height, 0, 0.5,
            0, 0, 1/depth, 0.5,
            0, 0, 0, 1)
    }

    Material {
        id: material
        effect: Effect {
        parameters: [
                Parameter {name: "backFaceMap"; value: backFaceMap},
                Parameter {name: "vertexToTex3DCoordMatrix"; value: d.vertexToTex3DCoordMatrix},
                Parameter {name: "volumeTexture"; value: volumeTexture},
                Parameter {name: "volumeColor"; value: volumeColor},
                Parameter {name: "lutParameters"; value: lutParameters},
                Parameter {name: "accumDivisor"; value: accumDivisor},
                Parameter {name: "visible"; value: visible ? 1.0 : 0.0}
            ]

            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 1
                    }
                    renderPasses: [
                        RenderPass {
                            filterKeys: [FilterKey {name: "pass"; value: "BackFace"}]
                            
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/qml/views/shaders/backfacemap.vert")
                                fragmentShaderCode: loadSource("qrc:/qml/views/shaders/backfacemap.frag")
                            }

                            renderStates: [
                                CullFace {mode: CullFace.Front}
                            ]
                        },

                        RenderPass {
                            filterKeys: [FilterKey {name: "pass"; value: "FrontFaceAccumulate"}]
                            
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/qml/views/shaders/volume.vert")
                                geometryShaderCode: loadSource("qrc:/qml/views/shaders/volume.geom")
                                fragmentShaderCode: loadSource("qrc:/qml/views/shaders/volume.frag")
                            }

                            renderStates: [
                                // DepthMask { mask: false },
                                BlendEquationArguments {
                                    sourceRgb: BlendEquationArguments.One
                                    destinationRgb: BlendEquationArguments.One
                                    sourceAlpha: BlendEquationArguments.One
                                    destinationAlpha: BlendEquationArguments.One
                                },
                                BlendEquation {
                                    blendFunction: BlendEquation.Add
                                }
                            ]
                        }
                    ]
                }
            ]
        }
    }

    CuboidMesh {
        id: mesh
        xExtent: width
        yExtent: height
        zExtent: depth
    }

    components: [mesh, material, layer]
}