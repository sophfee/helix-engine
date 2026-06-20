

#define PI 3.14159265359
#define M_PI PI

#define M_TAU 6.28318530718
#define TAU M_TAU

// Interleaved Gradient Noise
// https://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
float quick_hash(vec2 pos) {
    const vec3 magic = vec3(0.06711056f, 0.00583715f, 52.9829189f);
    return fract(magic.z * fract(dot(pos, magic.xy)));
}

#define PCF_FILTER_SAMPLES 16
vec2 poissonDisk[PCF_FILTER_SAMPLES] = vec2[PCF_FILTER_SAMPLES](
    vec2(-0.94201624, -0.39906216),
    vec2(+0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870),
    vec2(+0.34495938, +0.29387760),
    vec2(-0.91588581, +0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, +0.27676845),
    vec2(+0.97484398, +0.75648379),
    vec2(+0.44323325, -0.97511554),
    vec2(+0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(+0.79197514, +0.19090188),
    vec2(-0.24188840, +0.99706507),
    vec2(-0.81409955, +0.91437590),
    vec2(+0.19984126, +0.78641367),
    vec2(+0.14383161, -0.14100790)
);

