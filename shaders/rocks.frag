#version 430 core

in GS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
    vec3 rockColor;
} fs_in;

out vec4 FragColor;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform DirectionalLight uLight;
uniform vec3 uCameraPos;


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

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    for(int i = 0; i < 3; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}

void main()
{
    
    vec3 color = fs_in.rockColor;
    
    
    float stonePattern = fbm(fs_in.fragPos.xz * 15.0);
    vec3 darkStone = color * 0.7;
    vec3 lightStone = color * 1.15;
    color = mix(darkStone, lightStone, stonePattern);
    
    
    float strata = abs(sin(fs_in.fragPos.y * 30.0 + stonePattern * 10.0));
    color *= 0.85 + strata * 0.15;
    
    
    if (fs_in.height < 0.75) {
        float moss = noise(fs_in.fragPos.xz * 25.0);
        moss = smoothstep(0.6, 0.8, moss);
        vec3 mossColor = vec3(0.4, 0.6, 0.35);
        color = mix(color, mossColor, moss * 0.3);
    }
    
    
    vec3 diluted = mix(color, vec3(0.8), 0.35);
    float variation = noise(fs_in.fragPos.xz * 20.0);
    color = mix(diluted, color, 0.55 + variation * 0.45);
    
    
    color = floor(color * 10.0) / 10.0;
    
    
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-uLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    
    
    diff = smoothstep(0.2, 0.8, diff);
    
    vec3 ambient = vec3(0.65);
    vec3 diffuse = diff * uLight.color * 0.5;
    
    vec3 finalColor = (ambient + diffuse) * color;
    
    
    float edgeDarkening = 1.0;
    if (fs_in.texCoord.x < 0.15 || fs_in.texCoord.x > 0.85 ||
        fs_in.texCoord.y < 0.15 || fs_in.texCoord.y > 0.85) {
        edgeDarkening = 0.8;
    }
    finalColor *= edgeDarkening;
    
    FragColor = vec4(finalColor, 1.0);
}