import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

    property Texture2D backfaceMap

    parameters: [
        Parameter {name: "backfaceMap"; value: root.backfaceMap}
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
                        fragmentShaderCode: loadSource("qrc:/qml/shaders/volume.frag")
                    }

                    // using default render state
                }
            ]
        }
    ]
}