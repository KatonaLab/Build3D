import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0
import koki.a3dc 1.0

Material {
    id: root

    property bool thresholding: false
    property real threshold: 0.

    parameters: [
        Parameter {
            name: "teximage"
            value: VolumetricTexture {
                objectName: "objVol"
            }
        },
        Parameter {
            name: "threshold"
            value: root.threshold
        },
        Parameter {
            name: "thresholding"
            value: root.thresholding ? 1. : 0.
        },
        Parameter {
            name: "time"
            NumberAnimation on value {
                from: -1.
                to: 1.
                duration: 6000
                loops: Animation.Infinite
            }
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
                in vec2 vertexTexCoord;
                out vec3 worldPosition;
                out vec2 fragCoord;
                uniform mat4 modelMatrix;
                uniform mat4 mvp;

                void main()
                {
                    // Transform position, normal, and tangent to world coords
                    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

                    // Calculate vertex position in clip coordinates
                    gl_Position = mvp * vec4(worldPosition, 1.0);
                    fragCoord = vertexTexCoord;
                }"

            fragmentShaderCode: "#version 150 core
                out vec4 fragColor;
                in vec2 fragCoord;
                uniform sampler3D teximage;
                uniform float time;
                uniform float threshold;
                uniform float thresholding;
                void main()
                {
                    float value = texture(teximage, vec3(fragCoord, abs(time))).r;
                    value = mix(value, step(threshold, value), thresholding);
                    // value = pow(value, 0.8);
                    fragColor = vec4(vec3(value), 1.);
                    // fragColor = vec4(vec3(threshold), 1.);
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