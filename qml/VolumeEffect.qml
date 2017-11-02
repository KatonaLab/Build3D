import Qt3D.Core 2.0
import Qt3D.Render 2.0
import koki.katonalab.a3dc 1.0

Effect {
    id: root

    property Texture2D backfaceMap
    property VolumeParameters volumeParameter0
    property VolumeParameters volumeParameter1
    property VolumeParameters volumeParameter2
    property VolumeParameters volumeParameter3
    property matrix4x4 vertexToTex3DCoordMatrix

    parameters: [
        Parameter {name: "backfaceMap"; value: root.backfaceMap},

        Parameter {name: "vertexToTex3DCoordMatrix"; value: root.vertexToTex3DCoordMatrix},

        Parameter {name: "ch0texture"; value: root.volumeParameter0.texture},
        Parameter {name: "ch1texture"; value: root.volumeParameter1.texture},
        Parameter {name: "ch2texture"; value: root.volumeParameter2.texture},
        Parameter {name: "ch3texture"; value: root.volumeParameter3.texture},

        Parameter {name: "ch0color"; value: root.volumeParameter0.color},
        Parameter {name: "ch1color"; value: root.volumeParameter1.color},
        Parameter {name: "ch2color"; value: root.volumeParameter2.color},
        Parameter {name: "ch3color"; value: root.volumeParameter3.color},

        Parameter {name: "ch0cutParams"; value: root.volumeParameter0.cutParams},
        Parameter {name: "ch1cutParams"; value: root.volumeParameter1.cutParams},
        Parameter {name: "ch2cutParams"; value: root.volumeParameter2.cutParams},
        Parameter {name: "ch3cutParams"; value: root.volumeParameter3.cutParams}
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
                    filterKeys: [FilterKey {name: "renderStyle"; value: "backface"}]
                    
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/qml/shaders/backfacemap.vert")
                        fragmentShaderCode: loadSource("qrc:/qml/shaders/backfacemap.frag")
                    }

                    renderStates: [
                        // DepthTest {depthFunction: DepthTest.Greater},
                        CullFace {mode: CullFace.Front}
                    ]
                },

                RenderPass {
                    filterKeys: [FilterKey {name: "renderStyle"; value: "forward"}]
                    
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/qml/shaders/volume.vert")
                        geometryShaderCode: loadSource("qrc:/qml/shaders/volume.geom")
                        fragmentShaderCode: loadSource("qrc:/qml/shaders/volume.frag")
                    }

                    // using default render state
                }
            ]
        }
    ]
}