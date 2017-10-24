import Qt3D.Core 2.0
import Qt3D.Render 2.0

Effect {
    id: root

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
                    filterKeys: [FilterKey {name: "renderStyle"; value: "forward"}]
                    
                    shaderProgram: ShaderProgram {
                        vertexShaderCode: loadSource("qrc:/qml/shaders/coordmap.vert")
                        fragmentShaderCode: loadSource("qrc:/qml/shaders/coordmap.frag")
                    }

                    renderStates: [
                        DepthTest {depthFunction: DepthTest.Greater},
                        CullFace {mode: CullFace.NoCulling}
                    ]
                }
            ]
        }
    ]
}