#pragma once
#include <cmath>


/*
 * A vec3d is a 3d coordinate which is used to hold vertex data
 */


struct vec3d {

    float x, y, z;
    float w; // This extra 'dimension' is common to have in 3d rendering
    // It is useful to operate on the non normalized values before normalizing them, then divide by w to normalize
    // e.g when clipping (not done in this demo, sorry)
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/projection-matrix-GPU-rendering-pipeline-clipping
    // https://webglfundamentals.org/webgl/lessons/webgl-3d-perspective.html

    

    vec3d(float a, float b, float c, float d) { 
        x = a;
        y = b;
        z = c;
        w = d;
    }

    vec3d(float a, float b, float c) 
        : vec3d(a, b, c, 1) {}

    vec3d() 
        : vec3d(0, 0, 0, 1) {}

    // Returns euclidean distance from (0, 0, 0) to (x, y, z)
    float length() const {
        return sqrt(x * x + y * y + z * z);
    }

    // Normalizes the vector (makes the length 1)
    void normalize() {
        float l = length();
        this->x /= l;
        this->y /= l;
        this->z /= l;
    }

    // Returns a normalized vector
    vec3d normal() const {
        vec3d r = *this;
        r.normalize();
        return r;
    }

    // Algebraic Dot Product
    float dot(const vec3d& b) const {
        return (x * b.x + y * b.y + z * b.z);
    }

    // Cross product
    vec3d cross(const vec3d& b) const {
        return vec3d((y * b.z - z * b.y), (z * b.x - x * b.z), (x * b.y - y * b.x));
    }


    // vec3d - vec3d operations
    vec3d& operator+=(const vec3d& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    vec3d operator+(const vec3d& rhs) const {
        vec3d r;
        r.x = this->x + rhs.x;
        r.y = this->y + rhs.y;
        r.z = this->z + rhs.z;
        return r;
    }

    vec3d& operator-=(const vec3d& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    vec3d operator-(const vec3d& rhs) const {
        vec3d r;
        r.x = this->x - rhs.x;
        r.y = this->y - rhs.y;
        r.z = this->z - rhs.z;
        return r;
    }

    vec3d& operator*=(const vec3d& rhs) {
        this->x *= rhs.x;
        this->y *= rhs.y;
        this->z *= rhs.z;
        return *this;
    }

    vec3d operator*(const vec3d& rhs) const {
        vec3d r;
        r.x = this->x * rhs.x;
        r.y = this->y * rhs.y;
        r.z = this->z * rhs.z;
        return r;
    }

    vec3d& operator/=(const vec3d& rhs) {
        this->x /= rhs.x;
        this->y /= rhs.y;
        this->z /= rhs.z;
        return *this;
    }

    vec3d operator/(const vec3d& rhs) const {
        vec3d r;
        r.x = this->x / rhs.x;
        r.y = this->y / rhs.y;
        r.z = this->z / rhs.z;
        return r;
    }

    // vec3d - float operations
    vec3d& operator+=(const float& rhs) {
        this->x += rhs;
        this->y += rhs;
        this->z += rhs;
        this->w += rhs;
        return *this;
    }

    vec3d operator+(const float& rhs) const {
        vec3d r;
        r.x = this->x + rhs;
        r.y = this->y + rhs;
        r.z = this->z + rhs;
        r.w = this->w + rhs;
        return r;
    }

    vec3d& operator-=(const float& rhs) {
        this->x -= rhs;
        this->y -= rhs;
        this->z -= rhs;
        this->w -= rhs;
        return *this;
    }

    vec3d operator-(const float& rhs) const {
        vec3d r;
        r.x = this->x - rhs;
        r.y = this->y - rhs;
        r.z = this->z - rhs;
        r.w = this->w - rhs;
        return r;
    }

    vec3d& operator*=(const float& rhs) {
        this->x *= rhs;
        this->y *= rhs;
        this->z *= rhs;
        this->w *= rhs;
        return *this;
    }

    vec3d operator*(const float& rhs) const {
        vec3d r;
        r.x = this->x * rhs;
        r.y = this->y * rhs;
        r.z = this->z * rhs;
        r.w = this->w * rhs;
        return r;
    }

    vec3d& operator/=(const float& rhs) {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;
        this->w /= rhs;
        return *this;
    }

    vec3d operator/(const float& rhs) const {
        vec3d r;
        r.x = this->x / rhs;
        r.y = this->y / rhs;
        r.z = this->z / rhs;
        r.w = this->w / rhs;
        return r;
    }



};