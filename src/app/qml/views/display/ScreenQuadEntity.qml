import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {

    property Layer layer
    property Texture2D frontFaceAccumulatorMap

    PlaneMesh {
        id: planeMesh
        width: 2.0
        height: 2.0
    }

    Material {
        id: screenMaterial
        effect: Effect {
            parameters: [
                Parameter {
                    name: "accumulatorMap"
                    value: frontFaceAccumulatorMap
                }
            ]

            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 1
                    } // GraphicsApiFilter

                    renderPasses: [
                        RenderPass {
                            filterKeys: [FilterKey {name: "pass"; value: "Final"}]
                        
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/qml/views/display/shaders/final.vert")
                                fragmentShaderCode: loadSource("qrc:/qml/views/display/shaders/final.frag")
                            } // ShaderProgram

                            renderStates: [
                                CullFace {mode: CullFace.NoCulling}
                            ]

                        } // RenderPass
                    ] // renderPasses
                } // Technique
            ] // techniques
        } // Effect
    } // Material

    components: [planeMesh, screenMaterial, layer]
}