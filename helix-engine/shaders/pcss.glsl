
#pragma include "shaders/math.glsl"

float PCSS_AverageBlockDepth2D(in sampler2DShadow csmTexture, vec3 projCoords, float w_light, float bias) {
    int sampleCount = 16;
    float blockerSum = 0;
    int blockerCount = 0;

    vec2 texelSize = vec2(1.0) / vec2(textureSize(csmTexture, 0));

    float closestDepth = texture(csmTexture, vec3(projCoords.xy, projCoords.z - bias));
    float currentDepth = projCoords.z;
    //return currentDepth;
    float search_range = w_light * (currentDepth - 0.05) / currentDepth;
    //return search_range;
    if (search_range <= 0) {
        return 0.0;
    }

    int range = int(search_range);
    //return range / 10.0;
    for (int i = 0; i < PCF_FILTER_SAMPLES; i++) {

        float sampleDepth = texture(csmTexture, vec3(projCoords.xy + poissonDisk[i] * (search_range * texelSize), projCoords.z - bias));

        if (sampleDepth > 0.0) {
            blockerSum += sampleDepth;
            blockerCount++;
        }
    }


    if (blockerCount > 0) {
        return blockerSum / blockerCount;
    }
    else {
        return 0; //--> not in shadow~~~~
    }
}

float PCSS_AverageBlockDepthCube(in samplerCubeShadow csmTexture, float currentDepth, vec3 projCoords, vec3 normal, float w_light, float bias) {
    int sampleCount = 16;
    float blockerSum = 0;
    int blockerCount = 0;

    float closestDepth = texture(csmTexture, vec4(projCoords, currentDepth - bias));

    float search_range = w_light * (currentDepth - 0.05) / currentDepth;

    if (search_range <= 0) {
        return 0.0;
    }

    vec3 tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    if (length(tangent) < 0.001) {
        tangent = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
    }
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);


    for (int i = 0; i < PCF_FILTER_SAMPLES; i++) {
        vec3 coords = projCoords + (TBN * vec3(poissonDisk[i] * search_range, 0.0));
        float sampleDepth = texture(csmTexture, vec4(coords, currentDepth - bias), 0.0);

        //if (sampleDepth > 0.0) {
            blockerSum += sampleDepth;
            blockerCount++;
        //}
    }

    if (blockerCount > 0) {
        return blockerSum / blockerCount;
    }
    else {
        return 0; //--> not in shadow~~~~
    }
}

//http://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf
float PCSS_PenumbraWidth(float d_receiver, float d_blocker, float w_light) {
    return (d_receiver - d_blocker) * w_light / d_blocker;
}

float PCSS2D(in sampler2DShadow csmTexture, float lightSize, vec3 fragPosViewSpace, vec3 normal, mat4 lightSpaceMatrix, float bias) {
    // select cascade layer
    vec4 fragPosWorldSpace = vec4(fragPosViewSpace, 1.0);

    float depthValue = abs(fragPosViewSpace.z);

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPosWorldSpace.xyz, 1.0);

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.z;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;


    if (currentDepth > 1.0) {
        return 0.0;
    }

    float averageBlockDepth = PCSS_AverageBlockDepth2D(
    csmTexture,
    projCoords,
    lightSize,
    bias
    );

    if (averageBlockDepth <= 0.0) {
        return 0.0;
    }

    float penumbraWidth = PCSS_PenumbraWidth(
        projCoords.z,
        averageBlockDepth,
        lightSize
    );

    float filterRadius = max(((penumbraWidth * 0.0005) / currentDepth), 0.0001);

    mat2 disk_rotation;
    {
        float r = quick_hash(gl_FragCoord.xy) * TAU;
        float sr = sin(r);
        float cr = cos(r);
        disk_rotation = mat2(cr, -sr, sr, cr);
    }

    // PCF
    float shadow = 0.0;

    for(int x = 0; x < PCF_FILTER_SAMPLES; ++x) {
        float pcfDepth = texture(csmTexture, vec3(projCoords.xy + (poissonDisk[x] * filterRadius * disk_rotation), currentDepth - bias));
        shadow += pcfDepth;
    }
    shadow /= float(PCF_FILTER_SAMPLES);

    return shadow;
}

float PCSSCube(in samplerCubeShadow csmTexture, float lightSize, float farPlane, vec3 fragToLight, float bias) {

    float depthValue = length(fragToLight);
    vec3 projCoords = fragToLight;

    float currentDepth = projCoords.z / farPlane; // Normalize depth for cube map sampling

    if (currentDepth > 1.0) {
        return 0.0;
    }
    
    vec3 normal = normalize(fragToLight); // Assuming the normal is pointing towards the light source for point lights

    float averageBlockDepth = PCSS_AverageBlockDepthCube(
        csmTexture,
        currentDepth,
        projCoords,
        normal,
        lightSize,
        bias
    );

    if (averageBlockDepth <= 0.0) {
        return 0.0;
    }

    float penumbraWidth = PCSS_PenumbraWidth(
        currentDepth,
        averageBlockDepth,
        lightSize
    );

    float filterRadius = max(((penumbraWidth * 0.0005) / currentDepth), 0.0001);

    mat2 disk_rotation;
    {
        float r = quick_hash(gl_FragCoord.xy) * TAU;
        float sr = sin(r);
        float cr = cos(r);
        disk_rotation = mat2(cr, -sr, sr, cr);
    }
    
    vec3 tangent = normalize(cross(normal, vec3(0.0, 1.0, 0.0)));
    if (length(tangent) < 0.001) {
        tangent = normalize(cross(normal, vec3(1.0, 0.0, 0.0)));
    }
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // PCF
    float shadow = 0.0;
    for (int x = 0; x < PCF_FILTER_SAMPLES; ++x) {
        vec3 coords = projCoords + (TBN * vec3(poissonDisk[x] * filterRadius * disk_rotation, 0.0));
        coords.z /= farPlane; // Normalize depth for cube map sampling
        float pcfDepth = texture(csmTexture, vec4(coords, currentDepth - bias), 0.0);
        shadow += pcfDepth;
    }
    shadow /= float(PCF_FILTER_SAMPLES);
    return shadow;
}