#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <climits>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <numeric>
#include <queue>
#include <future>
#include <omp.h>

#include "linalg.h"

using Vec2f = linalg::vec<float, 2>;
using Vec3f = linalg::vec<float, 3>;
using Vec4f = linalg::vec<float, 4>;
using std::cout, std::endl;
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
float PI = 4 * atanf(1);

std::ostream& operator<<(std::ostream &os, const Vec3f v) {
    return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

const int TRIANGLE = 0;
const int SPHERE = 1;

struct SceneElement {
    int type;
    Vec3f p1, p2, p3;
    float r;
    int surface_index;
};

template<typename A, typename B>
std::ostream& operator<<(std::ostream &os, const std::pair<A, B> &p) { 
    return os << '(' << p.first << ", " << p.second << ')';
}

template<typename T_container, typename T = typename std::enable_if<!std::is_same<T_container, std::string>::value, typename T_container::value_type>::type>
std::ostream& operator<<(std::ostream &os, const T_container &v) { 
    os << '{';
    std::string sep;
    for (const T &x : v) os << sep << x, sep = ", ";
    return os << '}';
}

float random_uniform() {
    return std::uniform_real_distribution<float>(0,1)(rng);
}

Vec3f sample_unit_hemisphere() {
    float r1 = random_uniform();
    float r2 = random_uniform();

    float theta = 2 * PI * r1;
    float phi = acosf(sqrtf(1 - r2));

    return Vec3f{sinf(phi) * cosf(theta), sinf(phi) * sinf(theta), cosf(phi)};
}

Vec3f offset_ray_origin(Vec3f ray_position, Vec3f normal) {
    return ray_position + 0.001f * normal;
}

std::tuple<Vec3f, Vec3f> coordinate_system(Vec3f v1) {
    float sign = std::copysign(1.0f, v1.z);
    float a = -1 / (sign + v1.z);
    float b = v1.x * v1.y * a;
    return {Vec3f(1 + sign * v1.x * v1.x * a, sign * b, -sign * v1.x),
            Vec3f(b, sign + v1.y * v1.y * a, -v1.y)};
}

Vec3f from_local(Vec3f v, Vec3f n) {
    auto [x, y] = coordinate_system(n);
    return v.x * x + v.y * y + v.z * n;
}

Vec3f to_local(const Vec3f v, Vec3f n) { 
    auto [x, y] = coordinate_system(n);
    return Vec3f(dot(v, x), dot(v, y), dot(v, n)); 
}

Vec3f mirror_reflect(const Vec3f incident_dir, const Vec3f normal) {
    return incident_dir - 2 * dot(incident_dir, normal) * normal;
}

struct Photon {
    Vec3f position;
    Vec3f direction;
    Vec3f power;
    int surface_id;
};

float ETA_1 = 1.000293f;
float ETA_2 = 2.058f;

Vec3f photon_refract(Vec3f omega, Vec3f normal, bool entering = true) {
    float ETA_RATIO = entering ? ETA_1/ETA_2 : ETA_2/ETA_1;
    Vec3f a = ETA_RATIO * (omega - dot(omega, normal) * normal);
    Vec3f b = normal * sqrtf(1.0f - ETA_RATIO * ETA_RATIO * (1.0f - dot(omega, normal) * dot(omega, normal)));
    return -1.0f * (a + b);
}

Vec3f normal_sphere(SceneElement sphere, Vec3f position) {
    Vec3f normal = position - sphere.p1;
    return normalize(normal);
}

Vec3f tone_map_Aces(const Vec3f value) {
    Vec3f color = 0.6f * value;
    float A = 2.51;
    float B = 0.03;
    float C = 2.43;
    float D = 0.59;
    float E = 0.14;
    color = (color * (A * color + B)) / (color * (C * color + D) + E);
    return color;
}