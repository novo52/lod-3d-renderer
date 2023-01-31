#pragma once

struct vec2d
{
    float u, v;
    float w;

    vec2d(float a, float b, float c) {
        u = a;
        v = b;
        w = c;
    }

    vec2d(float a, float b)
        : vec2d(a, b, 1) {}

    vec2d()
        : vec2d(0, 0, 1) {}

    // vec2d - vec2d operations
    vec2d& operator+=(const vec2d& rhs) {
        this->u += rhs.u;
        this->v += rhs.v;
        this->w += rhs.w;
        return *this;
    }

    vec2d operator+(const vec2d& rhs) const {
        vec2d r;
        r.u = this->u + rhs.u;
        r.v = this->v + rhs.v;
        r.w = this->w + rhs.w;
        return r;
    }

    vec2d& operator-=(const vec2d& rhs) {
        this->u -= rhs.u;
        this->v -= rhs.v;
        this->w -= rhs.w;
        return *this;
    }

    vec2d operator-(const vec2d& rhs) const {
        vec2d r;
        r.u = this->u - rhs.u;
        r.v = this->v - rhs.v;
        r.w = this->w - rhs.w;
        return r;
    }

    vec2d& operator*=(const vec2d& rhs) {
        this->u *= rhs.u;
        this->v *= rhs.v;
        this->w *= rhs.w;
        return *this;
    }

    vec2d operator*(const vec2d& rhs) const {
        vec2d r;
        r.u = this->u * rhs.u;
        r.v = this->v * rhs.v;
        r.w = this->w * rhs.w;
        return r;
    }

    vec2d& operator/=(const vec2d& rhs) {
        this->u /= rhs.u;
        this->v /= rhs.v;
        this->w /= rhs.w;
        return *this;
    }

    vec2d operator/(const vec2d& rhs) const {
        vec2d r;
        r.u = this->u / rhs.u;
        r.v = this->v / rhs.v;
        r.w = this->w / rhs.w;
        return r;
    }

    // vec2d - float operations
    vec2d& operator+=(const float& rhs) {
        this->u += rhs;
        this->v += rhs;
        this->w += rhs;
        return *this;
    }

    vec2d operator+(const float& rhs) const {
        vec2d r;
        r.u = this->u + rhs;
        r.v = this->v + rhs;
        r.w = this->w + rhs;
        return r;
    }

    vec2d& operator-=(const float& rhs) {
        this->u -= rhs;
        this->v -= rhs;
        this->w -= rhs;
        return *this;
    }

    vec2d operator-(const float& rhs) const {
        vec2d r;
        r.u = this->u - rhs;
        r.v = this->v - rhs;
        r.w = this->w - rhs;
        return r;
    }

    vec2d& operator*=(const float& rhs) {
        this->u *= rhs;
        this->v *= rhs;
        this->w *= rhs;
        return *this;
    }

    vec2d operator*(const float& rhs) const {
        vec2d r;
        r.u = this->u * rhs;
        r.v = this->v * rhs;
        r.w = this->w * rhs;
        return r;
    }

    vec2d& operator/=(const float& rhs) {
        this->u /= rhs;
        this->v /= rhs;
        this->w /= rhs;
        return *this;
    }

    vec2d operator/(const float& rhs) const {
        vec2d r;
        r.u = this->u / rhs;
        r.v = this->v / rhs;
        r.w = this->w / rhs;
        return r;
    }

    
};