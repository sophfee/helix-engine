
#pragma include "shaders/frustum.glsl"
#pragma include "shaders/box.glsl"

bool FrustumBoxIntersect(Frustum frustum, Box box)
{
    for (int i = 0; i < 6; i++)
    {
        vec3 negative = mix(box.Min, box.Max, greaterThan(frustum.Planes[i].xyz, vec3(0.0)));
        float a = dot(vec4(negative, 1.0), frustum.Planes[i]);

        if (a < 0.0)
        {
            return false;
        }
    }

    return true;
}