#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
} gs_in[];

out GS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
    vec3 rockColor;
} gs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform vec3 uCameraPos;

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

vec3 hash33(vec3 p) {
    p = vec3(dot(p, vec3(127.1, 311.7, 74.7)),
             dot(p, vec3(269.5, 183.3, 246.1)),
             dot(p, vec3(113.5, 271.9, 124.6)));
    return fract(sin(p) * 43758.5453123);
}

void generateRock(vec3 basePos, vec3 normal, float height, vec2 uv) {
    if (height < 0.60 || height > 0.95) return;
    
    float density = hash(uv * 80.0);
    float threshold = mix(0.97, 0.92, smoothstep(0.65, 0.85, height));
    if (density > threshold) return;
    
    float distToCam = length(uCameraPos - basePos);
    if (distToCam > 100.0) return;
    
    vec3 randVec = hash33(vec3(uv * 100.0, height));
    float rockSize = 0.08 + randVec.x * 0.25;
    float elongation = 0.5 + randVec.y * 1.5;
    float rotation = randVec.z * 6.28318;
    
    
    float lodFactor = 1.0 - smoothstep(60.0, 100.0, distToCam);
    rockSize *= lodFactor;
    
    
    vec3 rockColor = mix(
        vec3(0.5, 0.48, 0.45),  
        vec3(0.55, 0.52, 0.48), 
        randVec.x
    );
    
    
    if (height > 0.75) {
        rockColor = mix(rockColor, vec3(0.6, 0.6, 0.62), (height - 0.75) * 2.0);
    }
    
    
    vec3 up = mix(vec3(0, 1, 0), normal, 0.5);
    vec3 right = normalize(cross(up, vec3(0, 0, 1)));
    if (length(right) < 0.1) right = normalize(cross(up, vec3(1, 0, 0)));
    vec3 forward = cross(up, right);
    
    
    right = cos(rotation) * right + sin(rotation) * forward;
    forward = cross(up, right);
    
    
    float w = rockSize;
    float h = rockSize * elongation;
    float d = rockSize * 0.8;
    
    
    vec3 deform1 = hash33(vec3(uv * 110.0, 1.0)) * 2.0 - 1.0;
    vec3 deform2 = hash33(vec3(uv * 120.0, 2.0)) * 2.0 - 1.0;
    
    
    vec3 v[8];
    v[0] = basePos + (-right * w - up * 0.1 - forward * d) * 0.5 + deform1 * 0.02;
    v[1] = basePos + (right * w - up * 0.1 - forward * d) * 0.5 + deform2 * 0.02;
    v[2] = basePos + (right * w - up * 0.1 + forward * d) * 0.5 - deform1 * 0.02;
    v[3] = basePos + (-right * w - up * 0.1 + forward * d) * 0.5 - deform2 * 0.02;
    v[4] = basePos + (-right * w * 0.7 + up * h - forward * d * 0.7) * 0.5 + deform1 * 0.03;
    v[5] = basePos + (right * w * 0.7 + up * h - forward * d * 0.7) * 0.5 + deform2 * 0.03;
    v[6] = basePos + (right * w * 0.7 + up * h + forward * d * 0.7) * 0.5 - deform1 * 0.03;
    v[7] = basePos + (-right * w * 0.7 + up * h + forward * d * 0.7) * 0.5 - deform2 * 0.03;
    

    #define EMIT_FACE(a, b, c, d) \
    { \
        vec3 n = normalize(cross(v[b] - v[a], v[c] - v[a])); \
        gs_out.normal = n; \
        gs_out.height = height; \
        gs_out.rockColor = rockColor; \
        \
        /* triangle 1 */ \
        gs_out.fragPos = v[a]; gl_Position = uProj * uView * vec4(v[a], 1.0); EmitVertex(); \
        gs_out.fragPos = v[b]; gl_Position = uProj * uView * vec4(v[b], 1.0); EmitVertex(); \
        gs_out.fragPos = v[c]; gl_Position = uProj * uView * vec4(v[c], 1.0); EmitVertex(); \
        EndPrimitive(); \
        \
        /* triangle 2 */ \
        gs_out.fragPos = v[a]; gl_Position = uProj * uView * vec4(v[a], 1.0); EmitVertex(); \
        gs_out.fragPos = v[c]; gl_Position = uProj * uView * vec4(v[c], 1.0); EmitVertex(); \
        gs_out.fragPos = v[d]; gl_Position = uProj * uView * vec4(v[d], 1.0); EmitVertex(); \
        EndPrimitive(); \
    }



    EMIT_FACE(0, 1, 2, 3);
    EMIT_FACE(4, 5, 6, 7);
    EMIT_FACE(0, 1, 5, 4);
    EMIT_FACE(1, 2, 6, 5);
    EMIT_FACE(2, 3, 7, 6);
    EMIT_FACE(3, 0, 4, 7);
}

void main() {
    vec3 basePos = gs_in[0].fragPos;
    vec3 normal = gs_in[0].normal;
    float height = gs_in[0].height;
    vec2 uv = gs_in[0].texCoord;
    
    generateRock(basePos, normal, height, uv);
}