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
    property VolumetricData volumeData

    property color volumeColor
    property vector4d lutParameters

    property Layer layer
    property Texture2D backFaceMap
    property Texture2D frontFaceAccumulatorMap

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
                Parameter {name: "frontFaceAccumulatorMap"; value: frontFaceAccumulatorMap},
                Parameter {name: "vertexToTex3DCoordMatrix"; value: d.vertexToTex3DCoordMatrix},
                Parameter {name: "volumeTexture"; value: VolumetricTexture {data: volumeData}},
                Parameter {name: "volumeColor"; value: volumeColor},
                Parameter {name: "lutParameters"; value: lutParameters}
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

                            // using default render state
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