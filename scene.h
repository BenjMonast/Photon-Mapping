#include "common.h"
#include "material.h"

struct Scene {
    Vec3f ip_bottom_left = {0.558156, -0, -0.0057560205};
    Vec3f ip_up_vector = {0, 0, 0.56031203};
    Vec3f ip_right_vector = {-0.56031203, 0, 0};

    Vec3f camera_position = Vec3f{0.278f, 0.8f, 0.2744f};
	float focal_distance = 0.8f;

    int image_height = 1024;
    int image_width = 1024;
    // int image_height = 256;
    // int image_width = 256;

    float light_x = 0.195f;
    float light_y = -0.355f;
    float light_z = 0.545f;
    float light_len_x = 0.16f;
    float light_len_y = 0.16f;
	float inv_light_area = 1.0f / light_len_x / light_len_y;
	Vec3f light_normal{0.0f, 0.0f, -1.0f};

	std::vector<Surface> surfaces = {
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.0f, 0.0f, -1.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.0f, 1.0f, 0.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f},
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.0f, 0.0f, 1.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{-1.0f, 0.0f, 0.0f},
			.albedo = Vec3f{0.0f, 0.2117f, 0.3765f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{1.0f, 0.0f, 0.0f},
			.albedo = Vec3f{0.996f, 0.7373f, 0.0667f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.0f, 0.0f, -1.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = CAUSTIC,
			.normal = Vec3f{-0.296209850f, 0.955122885f, 0.000000776f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f},
		},
		Surface{
			.type = CAUSTIC,
			.normal = Vec3f{0.0f, 0.0f, 1.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = CAUSTIC,
			.normal = Vec3f{0.955648909f, 0.294508341f, 0.000000239f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = CAUSTIC,
			.normal = Vec3f{0.301707575f, -0.953400513f, 0.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = CAUSTIC,
			.normal = Vec3f{-0.956165759f, -0.292825958f, -0.0f},
			.albedo = Vec3f{0.874000013f, 0.874000013f, 0.875000000f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{-0.285119946f, -0.958491845f, 0.0f},
			.albedo = Vec3f{0.839039981f, 0.839039981f, 0.839999974f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.953400053f, -0.301709030f, 0.0f},
			.albedo = Vec3f{0.839039981f, 0.839039981f, 0.839999974f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{-0.957826408f, 0.287347476f, 0.0f},
			.albedo = Vec3f{0.839039981f, 0.839039981f, 0.839999974f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.292826874f, 0.956165478f, -0.0f},
			.albedo = Vec3f{0.839039981f, 0.839039981f, 0.839999974f}
		},
		Surface{
			.type = LAMBERTIAN,
			.normal = Vec3f{0.0f, 0.0f, 1.0f},
			.albedo = Vec3f{0.839039981f, 0.839039981f, 0.839999974f}
		},
		Surface{
			.type = CAUSTIC
		},
	};

    std::vector<SceneElement> scene_elements = {
		SceneElement {
			.type = SPHERE,
			.p1 = {0.128f, -0.177000158f, 0.159999954f},
			.r = 0.10f,
			.surface_index = 16
		},
		SceneElement {
			.type = SPHERE,
			.p1 = {0.42f, -0.300000158f, 0.35f},
			.r = 0.10f,
			.surface_index = 16
		},
        // Light
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{light_x, light_y + light_len_y, light_z},
			.p2 = Vec3f{light_x + light_len_x, light_y, light_z},
			.p3 = Vec3f{light_x, light_y, light_z},
			.surface_index = 0
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{light_x, light_y + light_len_y, light_z},
			.p2 = Vec3f{light_x + light_len_x, light_y + light_len_y, light_z},
			.p3 = Vec3f{light_x + light_len_x, light_y, light_z},
			.surface_index = 0
		},
        // Back
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.559199989f, 0.000000040f},
			.p3 = Vec3f{0.000000133f, -0.559199989f, 0.000000040f},
			.surface_index = 1
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.559199989f, 0.548799932f},
			.p3 = Vec3f{0.555999935f, -0.559199989f, 0.000000040f},
			.surface_index = 1
		},
        // Bottom
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.000000040f},
			.p2 = Vec3f{0.555999935f, -0.559199989f, 0.000000040f},
			.p3 = Vec3f{0.555999935f, -0.000000119f, 0.000000040f},
			.surface_index = 2
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.000000040f},
			.p2 = Vec3f{0.555999935f, -0.000000119f, 0.000000040f},
			.p3 = Vec3f{0.000000133f, -0.000000119f, 0.000000040f},
			.surface_index = 2
		},
        // Left
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.555999935f, -0.000000119f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.000000119f, 0.000000040f},
			.p3 = Vec3f{0.555999935f, -0.559199989f, 0.000000040f},
			.surface_index = 3
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.555999935f, -0.000000119f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.559199989f, 0.000000040f},
			.p3 = Vec3f{0.555999935f, -0.559199989f, 0.548799932f},
			.surface_index = 3
		},
        // Right
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.000000040f},
			.p2 = Vec3f{0.000000133f, -0.000000119f, 0.000000040f},
			.p3 = Vec3f{0.000000133f, -0.000000119f, 0.548799932f},
			.surface_index = 4
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.559199989f, 0.000000040f},
			.p2 = Vec3f{0.000000133f, -0.000000119f, 0.548799932f},
			.p3 = Vec3f{0.000000133f, -0.559199989f, 0.548799932f},
			.surface_index = 4
		},
        // Top
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.000000119f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.559199989f, 0.548799932f},
			.p3 = Vec3f{0.000000133f, -0.559199989f, 0.548799932f},
			.surface_index = 5
		},
        SceneElement {
			.type = TRIANGLE,
			.p1 = Vec3f{0.000000133f, -0.000000119f, 0.548799932f},
			.p2 = Vec3f{0.555999935f, -0.000000119f, 0.548799932f},
			.p3 = Vec3f{0.555999935f, -0.559199989f, 0.548799932f},
			.surface_index = 5
		},
        // Left box
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.423000127f, -0.247000158f, 0.329999954f},
		// 	.p2 = Vec3f{0.423000127f, -0.246999890f, 0.000000040f},
		// 	.p3 = Vec3f{0.264999926f, -0.296000093f, 0.000000040f},
		// 	.surface_index = 6
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.423000127f, -0.247000158f, 0.329999954f},
		// 	.p2 = Vec3f{0.264999926f, -0.296000093f, 0.000000040f},
		// 	.p3 = Vec3f{0.264999926f, -0.296000093f, 0.329999954f},
		// 	.surface_index = 6
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.423000127f, -0.246999890f, 0.000000040f},
		// 	.p2 = Vec3f{0.423000127f, -0.247000158f, 0.329999954f},
		// 	.p3 = Vec3f{0.472000152f, -0.406000137f, 0.329999954f},
		// 	.surface_index = 8
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.472000152f, -0.406000137f, 0.329999954f},
		// 	.p2 = Vec3f{0.264999926f, -0.296000093f, 0.329999954f},
		// 	.p3 = Vec3f{0.313999921f, -0.455999970f, 0.329999954f},
		// 	.surface_index = 7
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.472000152f, -0.406000137f, 0.329999954f},
		// 	.p2 = Vec3f{0.423000127f, -0.247000158f, 0.329999954f},
		// 	.p3 = Vec3f{0.264999926f, -0.296000093f, 0.329999954f},
		// 	.surface_index = 7
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.313999921f, -0.455999970f, 0.329999954f},
		// 	.p2 = Vec3f{0.313999921f, -0.455999970f, 0.000000040f},
		// 	.p3 = Vec3f{0.472000152f, -0.406000137f, 0.000000040f},
		// 	.surface_index = 9
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.264999926f, -0.296000093f, 0.329999954f},
		// 	.p2 = Vec3f{0.264999926f, -0.296000093f, 0.000000040f},
		// 	.p3 = Vec3f{0.313999921f, -0.455999970f, 0.000000040f},
		// 	.surface_index = 10
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.423000127f, -0.246999890f, 0.000000040f},
		// 	.p2 = Vec3f{0.472000152f, -0.406000137f, 0.329999954f},
		// 	.p3 = Vec3f{0.472000152f, -0.406000137f, 0.000000040f},
		// 	.surface_index = 8
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.264999926f, -0.296000093f, 0.329999954f},
		// 	.p2 = Vec3f{0.313999921f, -0.455999970f, 0.000000040f},
		// 	.p3 = Vec3f{0.313999921f, -0.455999970f, 0.329999954f},
		// 	.surface_index = 10
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.313999921f, -0.455999970f, 0.329999954f},
		// 	.p2 = Vec3f{0.472000152f, -0.406000137f, 0.000000040f},
		// 	.p3 = Vec3f{0.472000152f, -0.406000137f, 0.329999954f},
		// 	.surface_index = 9
		// },
        // // Right box
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.p2 = Vec3f{0.082000092f, -0.225000143f, 0.165000007f},
		// 	.p3 = Vec3f{0.082000092f, -0.225000143f, 0.000000040f},
		// 	.surface_index = 11
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.p2 = Vec3f{0.082000092f, -0.225000143f, 0.000000040f},
		// 	.p3 = Vec3f{0.240000039f, -0.271999955f, 0.000000040f},
		// 	.surface_index = 11
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.290000081f, -0.113999903f, 0.000000040f},
		// 	.p2 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.p3 = Vec3f{0.240000039f, -0.271999955f, 0.000000040f},
		// 	.surface_index = 12
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.082000092f, -0.225000143f, 0.000000040f},
		// 	.p2 = Vec3f{0.130000070f, -0.064999968f, 0.165000007f},
		// 	.p3 = Vec3f{0.130000070f, -0.064999968f, 0.000000040f},
		// 	.surface_index = 13
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.082000092f, -0.225000143f, 0.000000040f},
		// 	.p2 = Vec3f{0.082000092f, -0.225000143f, 0.165000007f},
		// 	.p3 = Vec3f{0.130000070f, -0.064999968f, 0.165000007f},
		// 	.surface_index = 13
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.130000070f, -0.064999968f, 0.000000040f},
		// 	.p2 = Vec3f{0.290000081f, -0.114000171f, 0.165000007f},
		// 	.p3 = Vec3f{0.290000081f, -0.113999903f, 0.000000040f},
		// 	.surface_index = 14
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.290000081f, -0.113999903f, 0.000000040f},
		// 	.p2 = Vec3f{0.290000081f, -0.114000171f, 0.165000007f},
		// 	.p3 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.surface_index = 12
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.130000070f, -0.064999968f, 0.000000040f},
		// 	.p2 = Vec3f{0.130000070f, -0.064999968f, 0.165000007f},
		// 	.p3 = Vec3f{0.290000081f, -0.114000171f, 0.165000007f},
		// 	.surface_index = 14
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.130000070f, -0.064999968f, 0.165000007f},
		// 	.p2 = Vec3f{0.082000092f, -0.225000143f, 0.165000007f},
		// 	.p3 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.surface_index = 15
		// },
        // SceneElement {
		// 	.type = TRIANGLE,
		// 	.p1 = Vec3f{0.130000070f, -0.064999968f, 0.165000007f},
		// 	.p2 = Vec3f{0.240000039f, -0.271999955f, 0.165000007f},
		// 	.p3 = Vec3f{0.290000081f, -0.114000171f, 0.165000007f},
		// 	.surface_index = 15
		// }
        };
};

Scene scene;

Vec3f sample_light_position() {
    return Vec3f{scene.light_x + scene.light_len_x * random_uniform(), scene.light_y + scene.light_len_y * random_uniform(), scene.light_z};
}

Surface surface(SceneElement ele) {
	return scene.surfaces[ele.surface_index];
}

bool is_emitter(SceneElement ele) {
	return ele.surface_index == 0;
}