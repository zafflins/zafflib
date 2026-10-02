#include <zl/zlstd.h>
#include <math.h>


inline f32 CrossVec2(Vec2 a, Vec2 b) {
    return a.x * b.y - a.y* b.x;
}

inline Vec2 AddVec2(Vec2 a, Vec2 b) {
    return (Vec2){ a.x + b.x, a.y + b.y };
}

inline Vec2 SubVec2(Vec2 a, Vec2 b) {
    return (Vec2){ a.x - b.x, a.y - b.y };
}

inline Vec2 MulVec2(Vec2 a, f32 s) {
    return (Vec2){ a.x * s, a.y * s };
}

inline Vec2 DivVec2(Vec2 a, f32 s) {
    return (Vec2){ a.x / s, a.y / s };
}

inline f32 DotVec2(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

inline f32 LenSqVec2(Vec2 v) {
    return DotVec2(v, v);
}

inline f32 LenVec2(Vec2 v) {
    return sqrtf(LenSqVec2(v));
}

inline Vec2 NormVec2(Vec2 v) {
    f32 l = LenVec2(v);
    return (l > 0.0f) ? DivVec2(v, l) : (Vec2){0};
}

inline Vec2 RotVec2(f32 a, Vec2 v) {
    f32 rad = RADIANS(a);
    f32 s = sinf(rad);
    f32 c = cosf(rad);
    return (Vec2){
        v.x*c - v.y*s,
        v.y*s + v.x*c
    };
}


inline Vec3 AddVec3(Vec3 a, Vec3 b) {
    return (Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

inline Vec3 SubVec3(Vec3 a, Vec3 b) {
    return (Vec3){ a.x - b.x, a.y - b.y, a.z - b.z };
}

inline Vec3 MulVec3(Vec3 a, f32 s) {
    return (Vec3){ a.x * s, a.y * s, a.z * s };
}

inline Vec3 DivVec3(Vec3 a, f32 s) {
    return (Vec3){ a.x / s, a.y / s, a.z / s };
}

inline f32 DotVec3(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline Vec3 CrossVec3(Vec3 a, Vec3 b) {
    return (Vec3){
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

inline f32 LenSqVec3(Vec3 v) {
    return DotVec3(v, v);
}

inline f32 LenVec3(Vec3 v) {
    return sqrtf(LenSqVec3(v));
}

inline Vec3 NormVec3(Vec3 v) {
    f32 l = LenVec3(v);
    return (l > 0.0f) ? DivVec3(v, l) : (Vec3){0};
}

inline Vec3 RotVec3x(f32 a, Vec3 v) {
    f32 rad = RADIANS(a);
    f32 s = sinf(rad);
    f32 c = cosf(rad);
    return (Vec3){
        v.x,
        v.y*c - v.z*s,
        v.y*s + v.z*c
    };
}

inline Vec3 RotVec3y(f32 a, Vec3 v) {
    f32 rad = RADIANS(a);
    f32 s = sinf(rad);
    f32 c = cosf(rad);
    return (Vec3){
        v.x*c - v.z*s,
        v.y,
        v.x*s + v.z*c
    };
}

inline Vec3 RotVec3z(f32 a, Vec3 v) {
    f32 rad = RADIANS(a);
    f32 s = sinf(rad);
    f32 c = cosf(rad);
    return (Vec3){
        v.x*c - v.y*s,
        v.x*s + v.y*c,
        v.z
    };
}

// rodrigues' rotation:
// there are three terms
// v * cos
// axis x v * sin
// axis * axis * v * (1 - c)
//
// the first two terms correspond to x' and y' in:
// x' = x*cos - y*sin
// y' = x*sin + y*cos
//
// v*cos gives the x*cos and y*cos (the part of the rotated vector that remains aligned with v)
// while axis x v * sin gives x*sin and y*sin which is crossed with the axis to remain perpendicular
//
// the third term effectively undoes the rotation to the cos componet since it should remain aligned to v
// because cos + 1 - cos = 1
inline Vec3 RotateVec3(Vec3 v, Vec3 axis, f32 angle) {
    f32 s = sinf(angle);
    f32 c = cosf(angle);

    Vec3 parallel = MulVec3(axis, DotVec3(axis, v));
    Vec3 perpendicular = SubVec3(v, parallel);
    Vec3 perpendicular90 = CrossVec3(axis, perpendicular);

    return AddVec3(
        parallel,
        AddVec3(
            MulVec3(perpendicular, c),
            MulVec3(perpendicular90, s)
        )
    );
}

// inverse basis is just -rot and reversed multiplication (pitch along yaw)
inline void Vec3Basis(Vec3* rot, Vec3** basis) {
    f32 sinY = sinf(rot->x); f32 cosY = cosf(rot->x); // YAW
    f32 sinP = sinf(rot->y); f32 cosP = cosf(rot->y); // PITCH
    f32 sinR = sinf(rot->z); f32 cosR = cosf(rot->z);

    // compute basis vectors (TODO: move to rotation matrices)
    Vec3 ihatY = {cosY, 0, -sinY};
    Vec3 jhatY = {0, 1, 0};
    Vec3 khatY = {sinY, 0, cosY};

    Vec3 ihatP = {1, 0, 0};
    Vec3 jhatP = {0, cosP, sinP};
    Vec3 khatP = {0, -sinP, cosP};

    Vec3 ihatR = {cosR, -sinR, 0};
    Vec3 jhatR = {sinR, cosR, 0};
    Vec3 khatR = {0, 0, 1};

    // translate each coordinate along the corresponding basis vector:
    // given the basis (I J K), and a vector within the basis (X Y Z),
    // reconstruction is just (XI + YJ + ZK)
    Vec3 ihatYP = AddVec3(AddVec3(MulVec3(ihatY, ihatP.x), MulVec3(jhatY, ihatP.y)), MulVec3(khatY, ihatP.z));
    Vec3 jhatYP = AddVec3(AddVec3(MulVec3(ihatY, jhatP.x), MulVec3(jhatY, jhatP.y)), MulVec3(khatY, jhatP.z));
    Vec3 khatYP = AddVec3(AddVec3(MulVec3(ihatY, khatP.x), MulVec3(jhatY, khatP.y)), MulVec3(khatY, khatP.z));

    *basis[0] = AddVec3(AddVec3(MulVec3(ihatYP, ihatR.x), MulVec3(jhatYP, ihatR.y)), MulVec3(khatYP, ihatR.z));
    *basis[1] = AddVec3(AddVec3(MulVec3(ihatYP, jhatR.x), MulVec3(jhatYP, jhatR.y)), MulVec3(khatYP, jhatR.z));
    *basis[2] = AddVec3(AddVec3(MulVec3(ihatYP, khatR.x), MulVec3(jhatYP, khatR.y)), MulVec3(khatYP, khatR.z));
}


inline Vec4 AddVec4(Vec4 a, Vec4 b) {
    return (Vec4){ a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w };
}

inline Vec4 SubVec4(Vec4 a, Vec4 b) {
    return (Vec4){ a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w };
}

inline Vec4 MulVec4(Vec4 a, f32 s) {
    return (Vec4){ a.x*s, a.y*s, a.z*s, a.w*s };
}

inline Vec4 DivVec4(Vec4 a, f32 s) {
    return (Vec4){ a.x/s, a.y/s, a.z/s, a.w/s };
}

inline f32 DotVec4(Vec4 a, Vec4 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

inline f32 LenSqVec4(Vec4 v) {
    return DotVec4(v, v);
}

inline f32 LenVec4(Vec4 v) {
    return sqrtf(LenSqVec4(v));
}

inline Vec4 NormVec4(Vec4 v) {
    f32 l = LenVec4(v);
    return (l > 0.0f) ? DivVec4(v, l) : (Vec4){0};
}


Vec3 MulMat3Vec3(Vec3 v, Mat3 m) {
    return (Vec3){
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z
    };
}

Vec4 MulMat4Vec4(Vec4 v, Mat4 m) {
    return (Vec4){
        .x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
        .y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
        .z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w
    };
}
