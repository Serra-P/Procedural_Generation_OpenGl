#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

uniform sampler2D uHeightmap;
uniform float uHeightScale = 10.0;
uniform float uTerrainSize = 1280.0;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
} vs_out;

void main()
{
    
    vec2 uv = aPosition.xz / (uTerrainSize / 4) + 0.5;
    
    
    float h = texture(uHeightmap, uv).r;
    
    
    vec3 displacedPos = aPosition;
    displacedPos.y = (h - 0.5) * uHeightScale;
    
    vs_out.fragPos = vec3(uModel * vec4(displacedPos, 1.0));
    vs_out.normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vs_out.texCoord = uv;
    vs_out.height = h;
    
    gl_Position = uProj * uView * vec4(vs_out.fragPos, 1.0);
}