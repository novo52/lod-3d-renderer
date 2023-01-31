#include <corecrt_math.h>
#include "vec3d.cpp"
#define _USE_MATH_DEFINES
#include <math.h>


struct mat4x4 {
    float m[4][4] = { 0 }; // m[row][col];

    static mat4x4 projection(float fFovDegrees, float fAspectRatio, float fNear, float fFar) {
        float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * M_PI);
        float inv_far_near_difference = 1.0f / (fFar - fNear);

        mat4x4 proj;
        proj.m[0][0] = fAspectRatio * fFovRad;
        proj.m[1][1] = fFovRad;
        proj.m[2][2] = fFar * inv_far_near_difference;
        proj.m[3][2] = (-fFar * fNear) * inv_far_near_difference;
        proj.m[2][3] = 1.0f;
        return proj;
    }

    static mat4x4 identity() {
        mat4x4 id;
        id.m[0][0] = 1.0f;
        id.m[1][1] = 1.0f;
        id.m[2][2] = 1.0f;
        id.m[3][3] = 1.0f;
        return id;
    }

    static mat4x4 pointingAt(vec3d& pos, vec3d& target, vec3d& up)
    {
        // Calculate new forward direction
        vec3d forward = target - pos;
        forward.normalize();

        // Calculate new Up direction
        vec3d newUp = up - forward * up.dot(forward);
        newUp.normalize();

        // New Right direction is easy, its just cross product
        vec3d right = up.cross(forward);
        // No need to normalize, as up and forward were already normalized, so right must be too

        // Construct Dimensioning and Translation Matrix	
        mat4x4 m;
        m.m[0][0] = right.x;	m.m[0][1] = right.y;	m.m[0][2] = right.z;	m.m[0][3] = 0.0f;
        m.m[1][0] = newUp.x;	m.m[1][1] = newUp.y;	m.m[1][2] = newUp.z;	m.m[1][3] = 0.0f;
        m.m[2][0] = forward.x;	m.m[2][1] = forward.y;	m.m[2][2] = forward.z;	m.m[2][3] = 0.0f;
        m.m[3][0] = pos.x;		m.m[3][1] = pos.y;		m.m[3][2] = pos.z;		m.m[3][3] = 1.0f;
        return m;

    }

    mat4x4 quickInverse() const // Only for Rotation/Translation Matrices
    {
        mat4x4 r;
        r.m[0][0] = this->m[0][0]; r.m[0][1] = this->m[1][0]; r.m[0][2] = this->m[2][0]; r.m[0][3] = 0.0f;
        r.m[1][0] = this->m[0][1]; r.m[1][1] = this->m[1][1]; r.m[1][2] = this->m[2][1]; r.m[1][3] = 0.0f;
        r.m[2][0] = this->m[0][2]; r.m[2][1] = this->m[1][2]; r.m[2][2] = this->m[2][2]; r.m[2][3] = 0.0f;

        r.m[3][0] = -(this->m[3][0] * r.m[0][0] + this->m[3][1] * r.m[1][0] + this->m[3][2] * r.m[2][0]);
        r.m[3][1] = -(this->m[3][0] * r.m[0][1] + this->m[3][1] * r.m[1][1] + this->m[3][2] * r.m[2][1]);
        r.m[3][2] = -(this->m[3][0] * r.m[0][2] + this->m[3][1] * r.m[1][2] + this->m[3][2] * r.m[2][2]);
        r.m[3][3] = 1.0f;
        return r;
    }


    // vec3d - mat4x4 operations
    vec3d operator*(const vec3d& v) const {
        vec3d r;
        r.x = v.x * this->m[0][0] + v.y * this->m[1][0] + v.z * this->m[2][0] + this->m[3][0];
        r.y = v.x * this->m[0][1] + v.y * this->m[1][1] + v.z * this->m[2][1] + this->m[3][1];
        r.z = v.x * this->m[0][2] + v.y * this->m[1][2] + v.z * this->m[2][2] + this->m[3][2];
        r.w = v.x * this->m[0][3] + v.y * this->m[1][3] + v.z * this->m[2][3] + this->m[3][3];
        return r;
    }

    // mat4x4 -mat4x4 operations
    mat4x4 operator*(const mat4x4& rhs) const {
        mat4x4 r;
        for (int y = 0; y < 4; y++)
            for (int x = 0; x < 4; x++)
                r.m[y][x] = this->m[y][0] * rhs.m[0][x] +
                            this->m[y][1] * rhs.m[1][x] +
                            this->m[y][2] * rhs.m[2][x] +
                            this->m[y][3] * rhs.m[3][x];
        return r;
    }

    // One axis matrix rotations (rotation is calculated onto a matrix in one step)
    void applyRotationX(float theta) {
        float dx = cosf(theta);
        float dy = sinf(theta);

        for (int y = 0; y < 4; y++) {

            // No change for m[y][0]
            float my1 = m[y][1]; // Otherwise m[y][2] uses m[y][1] from after the previous calc
            m[y][1] = my1 * dx + m[y][2] * -dy;
            m[y][2] = my1 * dy + m[y][2] * dx;
            // No change for m[y][3]
        }
    }

    void applyRotationY(float theta) {
        float dx = cosf(theta);
        float dy = sinf(theta);

        for (int y = 0; y < 4; y++) {
            float my0 = m[y][0];
            m[y][0] = my0 * dx + m[y][2] * dy;
            // No change for m[y][1]
            m[y][2] = my0 * -dy + m[y][2] * dx;
            // No change for m[y][3]
        }
    }

    void applyRotationZ(float theta) {
        float dx = cosf(theta);
        float dy = sinf(theta);

        for (int y = 0; y < 4; y++) {
            float my0 = m[y][0];
            m[y][0] = my0 * dx + m[y][1] * -dy;
            m[y][1] = my0 * dy + m[y][1] * dx;
            // No change for m[y][2]
            // No change for m[y][3]
        }
    }

    // One step translation
    void applyTranslation(float dx, float dy, float dz) {
        for (int y = 0; y < 4; y++) {
            m[y][0] += m[y][3] * dx;
            m[y][1] += m[y][3] * dy;
            m[y][2] += m[y][3] * dz;
        }
    }

    // One step scaling
    void applyScaling(float dx, float dy, float dz) {
        for (int y = 0; y < 4; y++) {
            m[y][0] *= dx;
            m[y][1] *= dy;
            m[y][2] *= dz;
        }
    }
};

