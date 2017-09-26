import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {
    id: root

    property color maincolor: Qt.rgba(0.0, 1.0, 0.0, 1.0)

    parameters: [
        Parameter {
            name: "maincolor"
            value: Qt.vector3d(root.maincolor.r, root.maincolor.g, root.maincolor.b)
        }
    ]


    effect: Effect {

        FilterKey {
            id: forward
            name: "renderingStyle"
            value: "forward"
        }

        ShaderProgram {
            id: gl3Shader
            vertexShaderCode: "#version 150 core
                in vec3 vertexPosition;
                out vec3 worldPosition;
                uniform mat4 modelMatrix;
                uniform mat4 mvp;

                void main()
                {
                    // Transform position, normal, and tangent to world coords
                    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

                    // Calculate vertex position in clip coordinates
                    gl_Position = mvp * vec4(worldPosition, 1.0);
                }"

            fragmentShaderCode: "#version 150 core
                out vec4 fragColor;
                uniform vec3 maincolor;
                void main()
                {
                    //output color from material
                    fragColor = vec4(maincolor,1.0);
                }"
        }

        techniques: [
            // OpenGL 3.1
            Technique {
                filterKeys: [forward]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 1
                }
                renderPasses: RenderPass {
                    shaderProgram: gl3Shader
                }
            }
        ]
    }
}