#include "common.h"

const uint8_t LAMBERTIAN = 0;
const uint8_t CAUSTIC = 1;
const uint8_t GLOSSY = 2;

struct Surface {
    uint8_t type;
    Vec3f normal;
    Vec3f albedo;
};