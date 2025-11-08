#version 430 core
in GS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
    float grassHeight;
} fs_in;
out vec4 FragColor;
struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};
uniform DirectionalLight uLight;
uniform vec3 uCameraPos;
uniform float time;

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
void main()
{
    
    vec3 baseGreen = vec3(0.45, 0.65, 0.35);    
    vec3 darkGreen = vec3(0.3, 0.45, 0.2);     
    vec3 yellowGreen = vec3(0.65, 0.75, 0.35); 
    
    
    float t = fs_in.grassHeight;
    vec3 color = mix(darkGreen, baseGreen, t);
    
    
    float variation = noise(fs_in.fragPos.xz * 20.0 + vec2(time * 0.1));
    color = mix(color, yellowGreen, variation * 0.3 * t);
    
    
    if (t > 0.7) {
        color = mix(color, yellowGreen, (t - 0.7) * 2.0);
    }
    
    
    vec3 diluted = mix(color, vec3(0.85), 0.3);
    color = mix(diluted, color, 0.6 + variation * 0.4);
    
    
    color = floor(color * 8.0) / 8.0;
    
    
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-uLight.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    diff = smoothstep(0.0, 1.0, diff);
    
    vec3 ambient = vec3(0.5);
    vec3 diffuse = diff * uLight.color * 0.6;
    
    vec3 finalColor = (ambient + diffuse) * color;
    
    
    float alpha = 1.0;
    if (fs_in.texCoord.x < 0.1 || fs_in.texCoord.x > 0.9) {
        alpha = 0.8;
    }
    if (t > 0.95) {
        alpha = 0.9; 
    }
    
    FragColor = vec4(finalColor, alpha);
}
