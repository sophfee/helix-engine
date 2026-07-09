

struct PackedVec2 {
    float x;
    float y;
};

struct PackedVec3 {
    float x;
    float y;
    float z;
};

struct PackedVec4 {
    float x;
    float y;
    float z;
    float w;
};

PackedVec2 Pack(vec2 v) {
    return PackedVec2(v.x, v.y);
}

PackedVec3 Pack(vec3 v) {
    return PackedVec3(v.x, v.y, v.z);
}

PackedVec4 Pack(vec4 v) {
    return PackedVec4(v.x, v.y, v.z, v.w);
}

vec2 Unpack(PackedVec2 p) {
    return vec2(p.x, p.y);
}

vec3 Unpack(PackedVec3 p) {
    return vec3(p.x, p.y, p.z);
}

vec4 Unpack(PackedVec4 p) {
    return vec4(p.x, p.y, p.z, p.w);
}