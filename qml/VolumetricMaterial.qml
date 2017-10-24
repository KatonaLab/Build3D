import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.0
import koki.katonalab.a3dc 1.0

Material {
    id: root

    property bool thresholding: false
    property real threshold: 0.
    // property VolumetricData data: undefined

    parameters: [
        Parameter {
            name: "teximage"
            value: VolumetricTexture {
                // data: root.data
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
            // vertexShaderCode: "#version 150 core
            //     in vec3 vertexPosition;
            //     in vec2 vertexTexCoord;
            //     out vec3 worldPosition;
            //     out vec2 fragCoord;
            //     uniform mat4 modelMatrix;
            //     uniform mat4 mvp;

            //     void main()
            //     {
            //         // Transform position, normal, and tangent to world coords
            //         worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

            //         // Calculate vertex position in clip coordinates
            //         gl_Position = mvp * vec4(worldPosition, 1.0);
            //         fragCoord = vertexTexCoord;
            //     }"

            // fragmentShaderCode: "#version 150 core
            //     out vec4 fragColor;
            //     in vec2 fragCoord;
            //     uniform sampler3D teximage;
            //     uniform float time;
            //     uniform float threshold;
            //     uniform float thresholding;
            //     void main()
            //     {
            //         float value = texture(teximage, vec3(fragCoord, abs(time))).r * 0.1;
            //         value = mix(value, step(threshold, value), thresholding);
            //         // value = pow(value, 0.8);
            //         fragColor = vec4(vec3(value), 1.);
            //         // fragColor = vec4(vec3(threshold), 1.);
            //     }"

            vertexShaderCode: "#version 150 core
                in vec3 vertexPosition;
                out vec3 position;
                out vec3 direction;
                uniform mat4 viewMatrix;
                uniform mat4 modelMatrix;
                uniform mat4 modelViewMatrix;
                uniform mat4 modelViewProjection;
                uniform vec3 eyePosition;
                void main()
                {
                    position.xyz = vertexPosition.xyz + 0.5;
                    // position.z *= 15./512.;
                    vec4 d = modelMatrix * vec4(vertexPosition, 1.0) - vec4(eyePosition, 1.);
                    direction = d.xyz;
                    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);
                }"

            fragmentShaderCode: "#version 150 core
                out vec4 outputColor;
                in vec3 position;
                in vec3 direction;
                uniform sampler3D teximage;
                uniform float time;
                uniform float threshold;
                uniform float thresholding;
                void main()
                {
                    vec3 d = direction;
                    // vec3 d = direction;
                    float acc = 0.0;
                    float t = 0.;
                    for (int i = 0; i < 256; ++i) {
                        vec3 p = position + d * float(i)*0.015;
                        acc = acc + step(0.05, texture(teximage, p).r);
                        // 0.25 * 
                    }
                    outputColor = vec4(vec3(acc*0.05*0.5), 1.);
                    // outputColor = vec4(2.*texture(teximage, position).rrr, 1.);
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