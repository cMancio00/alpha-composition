#include "image.h"
#include "stb_image.h"


bool OpenMP_compose(const Image &foreground, Image &background) {
    if (foreground.height > background.height | foreground.width > background.width) {
        return false;
    }
#pragma omp parallel for collapse(2) shared(foreground)
    for (int y = 0; y < foreground.height; ++y) {
        for (int x = 0; x < foreground.width; ++x) {

            int backgroundIndex = (y * background.width + x) * STBI_rgb_alpha;
            int foregroundIndex = (y * foreground.width + x) * STBI_rgb_alpha;

            float alpha = static_cast<float>(foreground.rgb_image[foregroundIndex + 3]) / 255.0f;
            float beta = 1.0f - alpha;
#pragma omp simd
            for (int color = 0; color < 3; ++color) {
                background.rgb_image[backgroundIndex + color] =
                        static_cast<float>(background.rgb_image[backgroundIndex + color]) * beta
                        + static_cast<float>(foreground.rgb_image[foregroundIndex + color]) * alpha;

            }
        }
    }
    return true;
}

bool OpenACC_compose(const Image &foreground, Image &background) {
    if (foreground.height > background.height | foreground.width > background.width) {
        return false;
    }

    for (int y = 0; y < foreground.height; ++y) {
        for (int x = 0; x < foreground.width; ++x) {

            int backgroundIndex = (y * background.width + x) * STBI_rgb_alpha;
            int foregroundIndex = (y * foreground.width + x) * STBI_rgb_alpha;

            float alpha = static_cast<float>(foreground.rgb_image[foregroundIndex + 3]) / 255.0f;
            float beta = 1.0f - alpha;

            for (int color = 0; color < 3; ++color) {
                background.rgb_image[backgroundIndex + color] =
                        static_cast<float>(background.rgb_image[backgroundIndex + color]) * beta
                        + static_cast<float>(foreground.rgb_image[foregroundIndex + color]) * alpha;

            }
        }
    }
    return true;
}
