#include "common.h"

// Möller–Trumbore intersection algorithm
std::tuple<bool, float> ray_triangle_intersect(SceneElement triangle, Vec3f ray_origin, Vec3f ray_direction) {
    float eps = std::numeric_limits<float>::epsilon();

    Vec3f edge1 = triangle.p2 - triangle.p1;
    Vec3f edge2 = triangle.p3 - triangle.p1;
    float det = linalg::dot(edge1, linalg::cross(ray_direction, edge2));

    if (det < eps && det > eps) return {false, -1};

    float inv_det = 1.0 / det;
    Vec3f s = ray_origin - triangle.p1;
    float u = inv_det * linalg::dot(s, linalg::cross(ray_direction, edge2));

    if (u < 0 || u > 1) return {false, -1};

    float v = inv_det * linalg::dot(ray_direction, linalg::cross(s, edge1));

    if (v < 0 || u + v > 1) return {false, -1};

    float t = inv_det * linalg::dot(edge2, linalg::cross(s, edge1));

    if (t > eps) return {true, t};
    else return {false, -1};
}

std::tuple<bool, float> ray_sphere_intersect(const SceneElement &sphere, const Vec3f &ray_origin, const Vec3f ray_direction) {
    Vec3f V = ray_origin - sphere.p1;
	float a = dot(ray_direction, ray_direction);
    float b = 2.0f * dot(ray_direction, V);
    float c = dot(V, V) - sphere.r * sphere.r;
    float descriminant = b * b - 4.0f * a * c;

    if (descriminant < 0) return {false, -1.0f}; // 0 roots -> no intersection
    
    float t1 = ((-b) + sqrtf(descriminant)) / (2 * a);
    float t2 = ((-b) - sqrtf(descriminant)) / (2 * a);

    if (descriminant == 0) {
        if (t1 > 0) {
            return {true, t1};
        }
        return {false, -1.0f};
    }

    if (t1 <= 0 && t2 <= 0) return {false, -1.0f}; // the sphere is behind the ray
    
    if (t1 <= 0) return {true, t2};

    if (t2 <= 0) return {true, t1};

    return {true, std::min(t1, t2)};
}

std::tuple<bool, float, SceneElement> closest_hit(Vec3f ray_origin, Vec3f ray_direction, std::vector<SceneElement> &scene_elements) {
    float min_t_val = __FLT_MAX__;
    SceneElement hit;
    for (SceneElement ele : scene_elements) {
        bool any_hit;
        float t;
        if (ele.type == TRIANGLE) {
            std::tie(any_hit, t) = ray_triangle_intersect(ele, ray_origin, ray_direction);
        } else if (ele.type == SPHERE) {
            std::tie(any_hit, t) = ray_sphere_intersect(ele, ray_origin, ray_direction);
        }
        if (any_hit && t < min_t_val) {
            min_t_val = t;
            hit = ele;
        }
    }

    return {min_t_val != __FLT_MAX__, min_t_val, hit};
}