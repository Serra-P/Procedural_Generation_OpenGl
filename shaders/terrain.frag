#version 430 core
in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    float height;
} fs_in;

out vec4 FragColor;

struct DirectionalLight {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform DirectionalLight uLight;
uniform vec3 uBaseColor = vec3(1.0);
uniform vec3 uCameraPos;
uniform float uFogStart;
uniform float uFogEnd;
uniform vec3 uFogColor;

uniform float time;

// Si quelqu'un lit ce message bah fait pas gaffe au if succecif super moche
// Oui je sais c pas beau mais bon bah ca fonctionne et flemme

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
    for(int i = 0; i < 4; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }
    return value;
}



float detectEdge(float h, vec2 pos) {
    
    float edge = 0.0;
    
    for(int i = 0; i < 8; i++) {
        float angle = float(i) * 0.785398; 
        vec2 offset = vec2(cos(angle), sin(angle)) * 0.015;
        
        
        offset += vec2(noise(pos * 100.0 + vec2(float(i)))) * 0.01;
        
        float neighborH = h; 
        
        neighborH += fbm(pos + offset) * 0.3;
        
        edge += abs(h - neighborH);
    }
    
    return edge * 0.125;
}



vec3 getWatercolorTerrainColor(float h, vec2 pos) {
    
    vec2 turbulence = vec2(
        fbm(pos * 15.0) * 0.08,
        fbm(pos * 15.0 + vec2(5.2, 1.3)) * 0.08
    );
    pos += turbulence;
    
    
    float largePatch = fbm(pos * 4.0);
    float mediumPatch = fbm(pos * 12.0);
    
    
    h += largePatch * 0.35 + mediumPatch * 0.2;
    
    vec3 color;
    vec3 dilutedColor; 
    
    
    if (h < 0.30) {
        float t = smoothstep(0.0, 0.25, h);
        
        
        vec3 deepBlue = vec3(0.2, 0.45, 0.85);
        vec3 turquoise = vec3(0.3, 0.7, 0.9);
        vec3 lightBlue = vec3(0.5, 0.8, 0.95);
        
        
        vec3 dilutedDeep = mix(deepBlue, vec3(0.85), 0.4);
        vec3 dilutedTurq = mix(turquoise, vec3(0.9), 0.35);
        
        float waterPatch = fbm(pos * 18.0);
        color = mix(deepBlue, turquoise, t);
        dilutedColor = mix(dilutedDeep, dilutedTurq, t);
        
        
        color = mix(dilutedColor, color, 0.6 + waterPatch * 0.4);
    }
    
    else if (h < 0.32) {
        float t = smoothstep(0.25, 0.32, h);
        
        vec3 wetSand = vec3(0.85, 0.75, 0.55);
        vec3 drySand = vec3(0.98, 0.92, 0.7);
        
        vec3 dilutedWet = mix(wetSand, vec3(0.95), 0.3);
        vec3 dilutedDry = mix(drySand, vec3(0.98), 0.2);
        
        float sandPatch = fbm(pos * 22.0);
        color = mix(wetSand, drySand, t);
        dilutedColor = mix(dilutedWet, dilutedDry, t);
        
        color = mix(dilutedColor, color, 0.5 + sandPatch * 0.5);
    }
    
    else if (h < 0.65) {
        float t = smoothstep(0.32, 0.5, h);
        
        vec3 springGreen = vec3(0.55, 0.88, 0.45);
        vec3 meadowGreen = vec3(0.45, 0.78, 0.4);
        vec3 forestGreen = vec3(0.35, 0.68, 0.35);
        
        
        vec3 dilutedSpring = mix(springGreen, vec3(0.85), 0.35);
        vec3 dilutedMeadow = mix(meadowGreen, vec3(0.8), 0.4);
        
        float grassPatch = fbm(pos * 11.0);
        color = mix(springGreen, meadowGreen, grassPatch);
        dilutedColor = mix(dilutedSpring, dilutedMeadow, grassPatch);
        
        color = mix(dilutedColor, color, 0.55 + grassPatch * 0.45);
        color = mix(color, forestGreen, t * 0.3);
        
        
        float flowerPatch = step(0.75, noise(pos * 28.0));
        vec3 flowers = vec3(0.95, 0.4, 0.6);
        color = mix(color, flowers, flowerPatch * 0.35);
    }
    
    else if (h < 0.85) {
        float t = smoothstep(0.5, 0.7, h);
        
        vec3 olive = vec3(0.65, 0.7, 0.4);
        vec3 ochre = vec3(0.8, 0.68, 0.48);
        vec3 brownGreen = vec3(0.6, 0.55, 0.4);
        
        vec3 dilutedOlive = mix(olive, vec3(0.85), 0.3);
        vec3 dilutedOchre = mix(ochre, vec3(0.88), 0.25);
        
        float hillPatch = fbm(pos * 13.0);
        color = mix(olive, ochre, hillPatch);
        dilutedColor = mix(dilutedOlive, dilutedOchre, hillPatch);
        
        color = mix(dilutedColor, color, 0.5 + hillPatch * 0.5);
        color = mix(color, brownGreen, t * 0.6);
    }
    
    else if (h < 0.90) {
        float t = smoothstep(0.7, 0.85, h);
        
        vec3 rockBrown = vec3(0.55, 0.5, 0.45);
        vec3 stoneGray = vec3(0.6, 0.6, 0.65);
        vec3 darkGray = vec3(0.5, 0.5, 0.55);
        
        vec3 dilutedBrown = mix(rockBrown, vec3(0.8), 0.35);
        vec3 dilutedGray = mix(stoneGray, vec3(0.85), 0.3);
        
        float strata = abs(sin(h * 45.0 + fbm(pos * 9.0) * 6.0));
        color = mix(rockBrown, stoneGray, strata);
        dilutedColor = mix(dilutedBrown, dilutedGray, strata);
        
        color = mix(dilutedColor, color, 0.6 + strata * 0.4);
        color = mix(color, darkGray, t * 0.5);
    }
    
    else {
        float t = smoothstep(0.85, 1.0, h);
        
        vec3 snowShadow = vec3(0.8, 0.85, 0.92);
        vec3 snowBright = vec3(0.96, 0.97, 0.99);
        
        float snowPatch = fbm(pos * 28.0);
        color = mix(snowShadow, snowBright, t);
        color = mix(color, vec3(0.88, 0.92, 0.96), snowPatch * 0.4);
    }
    
    return color;
}



vec3 applyIrregularEdges(vec3 color, float h, vec2 pos) {
    float edge = detectEdge(h, pos);
    
    
    if(edge > 0.15) {
        
        float edgeNoise = noise(pos * 50.0);
        float darkening = mix(0.75, 0.85, edgeNoise);
        color *= mix(1.0, darkening, smoothstep(0.15, 0.4, edge));
        
        
        float lum = dot(color, vec3(0.299, 0.587, 0.114));
        color = mix(vec3(lum), color, 1.2 + edge * 0.3);
    }
    
    return color;
}

void main()
{    
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-uLight.direction);

    vec2 animatedUV = fs_in.texCoord + vec2(fbm(fs_in.texCoord * 2.0 + time * 0.05)) * 0.02;
    vec3 color = getWatercolorTerrainColor(fs_in.height, animatedUV);
    color = applyIrregularEdges(color, fs_in.height, animatedUV);

    float wave = fbm(animatedUV * 3.0 + vec2(time * 0.1, time * 0.07));
    color *= 1.0 + 0.2 * sin(time * 0.5 + wave * 6.2831);

    
    color = floor(color * 9.0) / 9.0;
    
    
    float diff = max(dot(normal, lightDir), 0.0);
    diff = smoothstep(0.0, 1.0, diff);
    
    vec3 diffuse = diff * uLight.color * uLight.intensity * 0.45;
    vec3 ambient = vec3(0.72);
    
    vec3 finalColor = (ambient + diffuse) * color * uBaseColor * 0.6;
    
    
    float dist = length(uCameraPos - fs_in.fragPos);
    float fogFactor = clamp((uFogEnd - dist) / (uFogEnd - uFogStart), 0.0, 1.0);
    vec3 foggedColor = mix(uFogColor, finalColor, fogFactor);
    
    FragColor = vec4(foggedColor, 1.0);
}
