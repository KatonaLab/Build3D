#version 150 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

// NOTE: important not to do perspective correct interpolation, 
// because it is in the screen space
noperspective out vec2 screenCoordGeom;
out vec3 tex3DCoordGeom;

uniform mat4 mvp; // = modelViewProjection
uniform mat4 vertexToTex3DCoordMatrix;
uniform mat4 inverseModelView;

// for testing purposes:
// const vec3 cutPlaneCenter = vec3(-0.4, 0.01, 0.0);
// const vec3 cutPlaneNormal = normalize(vec3(1.0, 0.8, 0.2));

vec3 cutPlaneCenter;
vec3 cutPlaneNormal;

struct CuttedTriangleData {
    vec4 p0, p1, p2, p01, p02, pExtra;
};

// expects that there is only one 'flags' element holding 'value'
ivec3 rollToValue(ivec3 flags, int value)
{
    ivec3 indices = ivec3(0, 1, 2);
    for (int i = 0; i < 2; ++i) {
        if (flags[i] == value) {
            break;
        }
        indices.xyz = indices.yzx;
    }
    // now, flags[indices[0]] holds the 'value'
    // all the indices are rolled
    return indices;
}

float intersect(vec3 planeNormal, vec3 planePoint, 
    vec3 lineNormal, vec3 linePoint)
{
    float a = dot(planePoint - linePoint, planeNormal);
    float b = dot(lineNormal, planeNormal);
    return a / b;
    // the point is at a/b*lineNormal + linePoint
}

CuttedTriangleData fillTriangleData(vec3 distances, ivec3 indices)
{
    vec3 d = vec3(distances[indices[0]], distances[indices[1]], distances[indices[2]]);

    CuttedTriangleData data;

    float r01 = d[0] / (d[0] + d[1]);
    float r02 = d[0] / (d[0] + d[2]);

    data.p0 = gl_in[indices[0]].gl_Position;
    data.p1 = gl_in[indices[1]].gl_Position;
    data.p2 = gl_in[indices[2]].gl_Position;
    data.p01 = mix(data.p0, data.p1, r01);
    data.p02 = mix(data.p0, data.p2, r02);
    data.pExtra = vec4(cutPlaneCenter, 1.0);

    return data;
}

void transformAndEmit(vec4 point)
{
    gl_Position = mvp * point;
    screenCoordGeom = (gl_Position.xyz / gl_Position.w * 0.5 + 0.5).xy;
    tex3DCoordGeom = (vertexToTex3DCoordMatrix * point).xyz;
    EmitVertex();
}

void oneVertexRemains(vec3 distances, ivec3 flags)
{
    ivec3 idx = rollToValue(ivec3(flags), 1);
    CuttedTriangleData d = fillTriangleData(distances, idx);

    //      p0
    //      /\ 
    // p02 /  \ p01 (+)
    //  ==========  cut
    //   /      \   (-)
    //  ----------
    // p2        p1

    transformAndEmit(d.p0);
    transformAndEmit(d.p01);
    transformAndEmit(d.p02);
    transformAndEmit(d.pExtra);
}

void twoVerticesRemain(vec3 distances, ivec3 flags)
{
    ivec3 idx = rollToValue(ivec3(flags), 0);
    CuttedTriangleData d = fillTriangleData(distances, idx);

    //   p1        p2
    //    ----------
    // p01 \      / p02 (+)
    //    ==========    cut
    //       \  /       (-)
    //        \/
    //        p0

    transformAndEmit(d.p1);
    transformAndEmit(d.p2);
    transformAndEmit(d.p01);
    transformAndEmit(d.p02);
    transformAndEmit(d.pExtra);
}

void emitOriginal()
{
    for (int i = 0; i < 3; ++i) {
        transformAndEmit(gl_in[i].gl_Position);
    }
}

void main()
{
    cutPlaneCenter = (inverseModelView * vec4(0., 0., -0.11, 1.)).xyz;
    cutPlaneNormal = (inverseModelView * vec4(0., 0., -1., 0.)).xyz;

    vec3 signedDistances;

    for (int i = 0; i < 3; ++i) {
        // positive value indicates that the vertex is 'in front' of the plane (+)
        // negative value indicates that the vertex is 'behind' the plane (-)
        signedDistances[i] = dot(gl_in[i].gl_Position.xyz - cutPlaneCenter, cutPlaneNormal);
    }

    vec3 flags = step(0.0, signedDistances);
    int flagSum = int(dot(flags, vec3(1.0)));
    vec3 distances = abs(signedDistances);

    // most of the time this is the case, returning early
    if (flagSum == 3) {
        emitOriginal();
        return;
    }

    if (flagSum == 1) {
        oneVertexRemains(distances, ivec3(flags));
        return;
    }

    if (flagSum == 2) {
        twoVerticesRemain(distances, ivec3(flags));
        return;
    }

    // flagSum == 0 -> we drop the triangle, it's behind the cutPlane
}
