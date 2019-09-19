#version 150 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

out vec4 screenCoordGeom;
out vec3 tex3DCoordGeom;

uniform mat4 mvp; // = modelViewProjection
uniform mat4 vertexToTex3DCoordMatrix;
uniform mat4 modelView;
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
    // screenCoordGeom = (gl_Position.xyz / gl_Position.w * 0.5 + 0.5).xy;
    // screenCoordGeom = gl_Position.xy / gl_Position.w;
    screenCoordGeom = gl_Position;
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

// tex3d space
const vec4 boxCorners[8] = vec4[](
    vec4(0.,0.,0.,1.),
    vec4(1.,0.,0.,1.),
    vec4(1.,1.,0.,1.),
    vec4(0.,1.,0.,1.),
    vec4(0.,0.,1.,1.),
    vec4(1.,0.,1.,1.),
    vec4(1.,1.,1.,1.),
    vec4(0.,1.,1.,1.));

// vertex space, the box center should be handed over through a uniform
const vec3 boxCenter = vec3(0.0);

void main()
{
    // note that near cut plane in SceneRootEntity is 0.1, we choose the offset of -0.11 as our geometry cut
    float cutZ = -0.11;

    // find a box corner that is behind the camera plane
    mat4 toCameraSpace = modelView * inverse(vertexToTex3DCoordMatrix);
    vec4 cornerBehindCamera = vec4(0.0);
    float farthestCornerBehindCamera = cutZ;
    for (int i = 0; i < 8; ++i) {
        vec4 p = toCameraSpace * boxCorners[i];
        p /= p.w;
        if (p.z > farthestCornerBehindCamera) { // behind the camera
            cornerBehindCamera = boxCorners[i];
            farthestCornerBehindCamera = p.z;
        }
    }
    
    cutPlaneNormal = (inverseModelView * vec4(0., 0., -1., 0.)).xyz;

    if (length(cornerBehindCamera) < 0.00001) {
        // the camera plane does not intersect the box, cornerBehindCamera is stayed its initial value of vec4(0.0)
        cutPlaneCenter = (inverseModelView * vec4(0.0, 0.0, cutZ, 1.)).xyz; 
    } else {

        // connect the box center with the found corner and intersect with the camera plane
        vec3 n = cutPlaneNormal;
        vec3 p1 = boxCenter;
        
        vec4 tmp = inverse(vertexToTex3DCoordMatrix) * cornerBehindCamera;
        tmp /= tmp.w;
        vec3 p0 = tmp.xyz;

        tmp = inverseModelView * vec4(0.0, 0.0, cutZ, 1.);
        tmp /= tmp.w;
        vec3 c = tmp.xyz;

        float denum = dot(p1 - p0, n);
        if (abs(denum) < 0.00001) {
            cutPlaneCenter = (inverseModelView * vec4(0.0, 0.0, cutZ, 1.)).xyz;
        } else {
            float a = (dot(c, n) - dot(p0, n)) / denum;
            cutPlaneCenter = p0 + a * (p1 - p0);
            // cutPlaneCenter = (inverseModelView * vec4(0.0, 0.0, cutZ, 1.)).xyz;
        }
    }

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
