import QtQuick 2.8
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import Qt3D.Input 2.0
import koki.katonalab.a3dc 1.0

Entity {
    id: root

    property vector3d size

    readonly property Buffer tex3DCoordsBuffer: Buffer {
        data: {
            return new Float32Array([
                0, 0, 0,  0, 1, 0,  0, 0, 1,  0, 1, 1, // neg x
                1, 0, 1,  1, 1, 1,  1, 0, 0,  1, 1, 0, // pos x
                1, 1, 1,  0, 1, 1,  1, 1, 0,  0, 1, 0, // pos y
                1, 0, 0,  0, 0, 0,  1, 0, 1,  0, 0, 1, // neg y
                1, 0, 1,  0, 0, 1,  1, 1, 1,  0, 1, 1, // pos z
                0, 0, 0,  1, 0, 0,  0, 1, 0,  1, 1, 0, // neg z
                ]);
        }
    }

    VolumeMaterial {
        id: material
    }
 
    GeometryRenderer {
        id: mesh
        instanceCount: 1 // we need one instance
        geometry: cubeGeo
    }

    CuboidGeometry {
        id: cubeGeo
        xExtent: size.x
        yExtent: size.y
        zExtent: size.z
        attributes: [
            Attribute {
                attributeType: Attribute.VertexAttribute
                vertexBaseType: Attribute.Float
                vertexSize: 3 // we need 3 floats
                byteStride: 3 * 4 // a float is 4 bytes, so 12 bytes
                byteOffset: 0
                name: "tex3DCoords"
                buffer: tex3DCoordsBuffer
            }
        ]
    }

    components: [mesh, material]
}