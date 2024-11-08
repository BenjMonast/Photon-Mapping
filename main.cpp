#include "common.h" 
#include "scene.h"
#include "raytracer.h"
#include "kdtree.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

const int NUM_PHOTONS = 100000;
const float LIGHT_POWER = 1;
const float GLOSSY_CONSTANT = 0.1;
const int K = 500;
const int SPP = 1024;

std::vector<Photon> diffuse_photons;
std::vector<Photon> caustic_photons;
KDTree diffuse_kd;
KDTree caustic_kd;

void photon_trace(Vec3f ray_origin, Vec3f ray_direction, Vec3f incoming_power, bool diffuse = false, bool caustic = false) {
    auto [hit, t, ele] = closest_hit(ray_origin, ray_direction, scene.scene_elements);

    if (!hit) return;

    Vec3f normal;
    if (ele.type == TRIANGLE) {
        normal = surface(ele).normal;
    } else if (ele.type == SPHERE) {
        normal = normal_sphere(ele, ray_origin + ray_direction * t);
    }
	Vec3f albedo = surface(ele).albedo;

	if (surface(ele).type == LAMBERTIAN) {
        if (diffuse && !caustic) {
            diffuse_photons.push_back(Photon{ray_origin + t * ray_direction, -ray_direction, incoming_power, ele.surface_index});
        } else if (caustic && !diffuse) {
            caustic_photons.push_back(Photon{ray_origin + t * ray_direction, -ray_direction, incoming_power, ele.surface_index});
        }
        Vec3f albedo = surface(ele).albedo;
        float p_rr = (albedo.x + albedo.y + albedo.z) / 3.0f;
		if (random_uniform() < p_rr) { // Diffusely Reflected
			ray_origin = offset_ray_origin(ray_origin + t * ray_direction, normal);
			ray_direction = from_local(sample_unit_hemisphere(), normal);
			photon_trace(ray_origin, ray_direction, incoming_power * albedo / p_rr, true, caustic);
		}
	} else if (surface(ele).type == CAUSTIC) {
        if (dot(normal, ray_direction) > 0) { // Hit from behind
            ray_origin = offset_ray_origin(ray_origin + t * ray_direction, normal);
            ray_direction = photon_refract(-ray_direction, -normal, false);
        } else { // Hit from front
            ray_origin = offset_ray_origin(ray_origin + t * ray_direction, -normal);
            ray_direction = photon_refract(-ray_direction, normal);
        }
        photon_trace(ray_origin, ray_direction, incoming_power, diffuse, true);
	}
}

void map_photons() {
    for (int i = 0; i < NUM_PHOTONS; i++) {
        if (i % 100000 == 0) {
            cout << i << endl;
        }
        Vec3f ray_origin = offset_ray_origin(sample_light_position(), scene.light_normal);
        Vec3f ray_direction = from_local(sample_unit_hemisphere(), scene.light_normal);

        photon_trace(ray_origin, ray_direction, Vec3f{LIGHT_POWER, LIGHT_POWER, LIGHT_POWER}/NUM_PHOTONS);
    }
}

Vec3f eval_direct_lighting(Vec3f p, SceneElement ele) {
    Surface s = surface(ele);
    if (s.type != LAMBERTIAN) return Vec3f{0.0f, 0.0f, 0.0f};

    Vec3f point_on_light = sample_light_position();
    Vec3f shadow_ray_direction = normalize(point_on_light - p);
    auto [shadow_hit, shadow_t, shadow_ele] = closest_hit(p, shadow_ray_direction, scene.scene_elements);

    if (!is_emitter(shadow_ele)) return Vec3f{0.0f, 0.0f, 0.0f};

    Vec3f brdf = s.albedo / PI;
    Vec3f L_i = dot(-shadow_ray_direction, scene.light_normal) > 0 ? Vec3f{LIGHT_POWER * scene.inv_light_area, LIGHT_POWER * scene.inv_light_area, LIGHT_POWER * scene.inv_light_area} : Vec3f{0.0f, 0.0f, 0.0f};
    float pdf_light = length2(p - point_on_light) * scene.inv_light_area / dot(scene.light_normal, normalize(p - point_on_light));

    return brdf * L_i * dot(shadow_ray_direction, s.normal) / pdf_light;
}

Vec3f eval_indirect_lighting(Vec3f p, SceneElement ele) {
    Surface s = surface(ele);
    if (s.type != LAMBERTIAN) return Vec3f{0.0f, 0.0f, 0.0f};

    NNQ nnq;
    diffuse_kd.locate_photons(p, K, ele.surface_index, nnq);

    if (nnq.empty()) return Vec3f{0,0,0};

    float r = nnq.top().first;

    Vec3f total_flux{0,0,0};

    Vec3f brdf;
    brdf = surface(ele).albedo / PI;
    while (!nnq.empty()) {
        total_flux += diffuse_photons[nnq.top().second].power * brdf;
        nnq.pop();
    }

    Vec3f L_r = total_flux / (PI * r * r);

    return L_r;

}

Vec3f eval_caustic_lighting(Vec3f p, SceneElement ele) {
    Surface s = surface(ele);
    if (s.type != LAMBERTIAN) return Vec3f{0.0f, 0.0f, 0.0f};

    NNQ nnq;
    caustic_kd.locate_photons(p, K, ele.surface_index, nnq);

    if (nnq.empty()) return Vec3f{0,0,0};

    float r = nnq.top().first;

    Vec3f total_flux{0,0,0};

    Vec3f brdf;
    brdf = surface(ele).albedo / PI;
    while (!nnq.empty()) {
        total_flux += caustic_photons[nnq.top().second].power * brdf;
        nnq.pop();
    }

    Vec3f L_r = total_flux / (PI * r * r);

    return L_r;

}

Vec3f shade(Vec3f camera_position, Vec3f ray_direction, int i = -1, bool inside = false) {
    auto [hit, t, ele] = closest_hit(camera_position, ray_direction, scene.scene_elements);
    if (!hit) return Vec3f{0.0f, 0.0f, 0.0f};
    if (is_emitter(ele)) return Vec3f{1.0f, 1.0f, 1.0f};

    Vec3f hit_point = camera_position + t * ray_direction;

    Vec3f normal;
    if (ele.type == SPHERE) {
        normal = normal_sphere(ele, hit_point);
    } else {
        normal = surface(ele).normal;
    }

    if (surface(ele).type == CAUSTIC) {
        Vec3f L_r_specular{0.0f, 0.0f, 0.0f};
        if (!inside) {
            Vec3f mirror_ray_origin, mirror_ray_direction;
            if (dot(normal, ray_direction) > 0) { // Hit from behind
                mirror_ray_origin = offset_ray_origin(hit_point, -normal);
                ray_direction = mirror_reflect(ray_direction, -normal);
            } else { // Hit from front
                mirror_ray_origin = offset_ray_origin(hit_point, normal);
                mirror_ray_direction = mirror_reflect(ray_direction, normal);
            }
            L_r_specular = shade(mirror_ray_origin, mirror_ray_direction, i) * 0.05f;
        }
        Vec3f ray_origin;
        if (dot(normal, ray_direction) > 0) { // Hit from behind
            ray_origin = offset_ray_origin(hit_point, normal);
            ray_direction = photon_refract(-ray_direction, -normal, false);
        } else { // Hit from front
            ray_origin = offset_ray_origin(hit_point, -normal);
            ray_direction = photon_refract(-ray_direction, normal);
        }
        return L_r_specular + shade(ray_origin, ray_direction, i, !inside);
    }

    hit_point = offset_ray_origin(hit_point, normal);
    Vec3f L_r_direct{0.0f, 0.0f, 0.0f};
    if (surface(ele).type == LAMBERTIAN) {
        L_r_direct = eval_direct_lighting(hit_point, ele);
    }
    Vec3f L_r_indirect{0.0f, 0.0f, 0.0f};
    Vec3f L_r_caustic{0.0f, 0.0f, 0.0f};
    if (i == 0) {
        L_r_indirect = eval_indirect_lighting(hit_point, ele) * SPP;
        L_r_caustic = eval_caustic_lighting(hit_point, ele) * SPP;
    } else if (i == -1) {
        L_r_indirect = eval_indirect_lighting(hit_point, ele);
        L_r_caustic = eval_caustic_lighting(hit_point, ele);
    }

    return L_r_direct + L_r_indirect + L_r_caustic;
}

void visualize_photons(std::vector<Photon> &photons, char const * filename) {
    std::vector<bool> photon_selected(photons.size(), false);

    NNQ nnq;
    KDTree kd(&photons);
    kd.balance();
    kd.locate_photons(Vec3f{0.27,-0.56,0.27}, K, 1, nnq);
    while (!nnq.empty()) {
        photon_selected[nnq.top().second] = true;
        nnq.pop();
    }

    std::vector<Vec3f> pixels(scene.image_width * scene.image_height, Vec3f{0,0,0});
    for (int i = 0; i < photons.size(); i++) {
        Photon p = photons[i];
        Vec3f normalized_photon_position = (p.position - scene.camera_position);
        Vec3f position_on_image_plane = 0.8f * normalized_photon_position / normalized_photon_position.y + scene.ip_up_vector / 2 - scene.ip_right_vector / 2;
        int y = scene.image_height * position_on_image_plane.z / linalg::length(scene.ip_up_vector);
        int x = scene.image_width * position_on_image_plane.x / linalg::length(scene.ip_up_vector);
        if (y * scene.image_width + x < pixels.size())
            pixels[y * scene.image_width + x] += Vec3f{0.5,0.5,0.5};
        
    }

    std::vector<uint8_t> data(3 * scene.image_width * scene.image_height);
    for (int i = 0; i < scene.image_width * scene.image_height; i++) {
        for (int j = 0; j < 3; j++) {
            data[3 * i + j] = (uint8_t)(255.0f * std::min(1.0f, pixels[i][j]));
        }
    }
    stbi_write_png(filename, scene.image_width, scene.image_height, 3, data.data(), 3 * scene.image_width);
}

int main() {
    std::cout << "Starting Photon Mapping" << std::endl;

    map_photons();

    caustic_kd = KDTree(&caustic_photons);
    caustic_kd.balance();

    diffuse_kd = KDTree(&diffuse_photons);
    diffuse_kd.balance();

    visualize_photons(caustic_photons, "caustic.png");
    visualize_photons(diffuse_photons, "diffuse.png");

    std::cout << "Photon Mapping Complete" << std::endl;

    std::vector<Vec3f> pixels(scene.image_width * scene.image_height, Vec3f{-1.0f, -1.0f, -1.0f});

    std::cout << "Rendering Starting" << std::endl;

    int chunk_height = scene.image_height / 16;
    for (int i = 0; i < 16; i++) {
        #pragma omp parallel for
        for (int y = chunk_height * i; y < chunk_height * i + chunk_height; y++) {
            if (y % 5 == 0) {
                std::cout << "Rendering Row " << y << std::endl;
            }
            for (int x = 0; x < scene.image_width; x++) {
                pixels[y * scene.image_width + x] = Vec3f{0.0f, 0.0f, 0.0f};
                for (int i = 0; i < SPP; i++) {
                    float u = ((float)x + random_uniform())/scene.image_width;
                    float v = ((float)y + random_uniform())/scene.image_height;
                    Vec3f position_on_image_plane = scene.ip_bottom_left + u * scene.ip_right_vector + (1.0f - v) * scene.ip_up_vector;
                    Vec3f ray_direction = linalg::normalize(position_on_image_plane - scene.camera_position);
                    pixels[y * scene.image_width + x] += shade(scene.camera_position, ray_direction, i);
                }
                pixels[y * scene.image_width + x] /= (float)SPP;
                pixels[y * scene.image_width + x] = pixels[y * scene.image_width + x];
            }
        }
        cout << "Chunk " << i + 1 << "/16 complete" << endl;
    }

    std::vector<uint8_t> data(4 * scene.image_width * scene.image_height);
    for (int i = 0; i < scene.image_width * scene.image_height; i++) {
        if (pixels[i][0] == -1.0f) {
            for (int j = 0; j < 3; j++) {
                data[4 * i + j] = 0;
            }
            data[4 * i + 3] = 0;
            continue;
        }
        Vec3f pixel = tone_map_Aces(pixels[i]);
        for (int j = 0; j < 3; j++) {
            data[4 * i + j] = (uint8_t)(255.0f * std::max(0.0f,std::min(1.0f,pixel[j])));
        }
        data[4 * i + 3] = 255;
    }
    stbi_write_png("output.png", scene.image_width, scene.image_height, 4, data.data(), 4 * scene.image_width);
    // std::cout << "Success" << std::endl;
}