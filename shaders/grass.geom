#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 64) out;

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
    float grassHeight;
} gs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform float time;
uniform vec3 uCameraPos;


// Et oui grace a ce code plus besoin de sortir toucher de l'herbe on peu le faire dans opengl

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}


void emitGrassBlade(vec3 basePos, vec3 up, vec3 right, vec3 forward, float randHeight, float randWidth, float bendAmount, float angleOffset, vec3 windOffset, float height) {
    
    mat3 rot = mat3(
        cos(angleOffset), 0.0, sin(angleOffset),
        0.0, 1.0, 0.0,
        -sin(angleOffset), 0.0, cos(angleOffset)
    );
    right = normalize(rot * right);
    forward = normalize(rot * forward);

    
    vec3 tip = basePos + up * randHeight + forward * bendAmount * 0.2 + windOffset * 0.3;
    vec3 p0 = basePos - right * randWidth * 0.5;
    vec3 p1 = basePos + right * randWidth * 0.5;
    vec3 p2 = tip - right * randWidth * 0.3;
    vec3 p3 = tip + right * randWidth * 0.3;

    vec3 bladeNormal = normalize(cross(p1 - p0, p2 - p0));

    
    gs_out.height = height;

    
    gs_out.fragPos = p0;
    gs_out.normal = bladeNormal;
    gs_out.texCoord = vec2(0, 0);
    gs_out.grassHeight = 0.0;
    gl_Position = uProj * uView * vec4(p0, 1.0);
    EmitVertex();

    gs_out.fragPos = p1;
    gs_out.texCoord = vec2(1, 0);
    gs_out.grassHeight = 0.0;
    gl_Position = uProj * uView * vec4(p1, 1.0);
    EmitVertex();

    gs_out.fragPos = p2;
    gs_out.texCoord = vec2(0, 1);
    gs_out.grassHeight = 1.0;
    gl_Position = uProj * uView * vec4(p2, 1.0);
    EmitVertex();
    EndPrimitive();

    
    gs_out.fragPos = p1;
    gs_out.texCoord = vec2(1, 0);
    gl_Position = uProj * uView * vec4(p1, 1.0);
    EmitVertex();

    gs_out.fragPos = p3;
    gs_out.texCoord = vec2(1, 1);
    gs_out.grassHeight = 1.0;
    gl_Position = uProj * uView * vec4(p3, 1.0);
    EmitVertex();

    gs_out.fragPos = p2;
    gs_out.texCoord = vec2(0, 1);
    gl_Position = uProj * uView * vec4(p2, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main() {
    vec3 basePos = gs_in[0].fragPos;
    vec3 normal = gs_in[0].normal;
    float height = gs_in[0].height;
    vec2 uv = gs_in[0].texCoord;

    
    if (height < 0.32 || height > 0.40) return;

    
    vec3 up = mix(vec3(0, 1, 0), normal, 0.25);
    vec3 right = normalize(cross(up, vec3(0, 0, 1)));
    if (length(right) < 0.1) right = normalize(cross(up, vec3(1, 0, 0)));
    vec3 forward = cross(right, up);

    
    vec2 windDir = normalize(vec2(1.0, 0.4));
    float w = noise(basePos.xz * 0.05 + time * 0.15);
    vec3 windOffset = (right * 0.5 + forward * 0.5) * w * 0.4;

    
    float dist = length(uCameraPos - basePos);
    if (dist > 100.0) return;
    float lod = 1.0 - smoothstep(60.0, 100.0, dist);

    
    
    int blades = 6; 
    float randBaseHeight = 0.12 + hash(uv * 90.0) * 0.2;

    for (int i = 0; i < blades; i++) {
        float angleOffset = (float(i) / float(blades)) * 6.28318; 
        float randH = randBaseHeight + hash(uv + float(i)) * 0.1;
        float randW = 0.02 + hash(uv * 120.0 + float(i)) * 0.015;
        float bend = 0.15 + hash(uv * 80.0 + float(i)) * 0.3;
        emitGrassBlade(basePos, up, right, forward, randH * lod, randW, bend, angleOffset, windOffset, height);
    }
}
