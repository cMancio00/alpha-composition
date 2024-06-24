#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif //STB_IMAGE_IMPLEMENTATION

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif //STB_IMAGE_WRITE_IMPLEMENTATION
#include "image.h"
#include <string>
#include <format>
#include <vector>
#include <iostream>

/*
 * Load a single image. If image is not RGBA, alpha channel is assumed completely opaque (255).
 */
void Image::loadImage(const std::string &image_path, Image &image) {
    image.rgb_image = stbi_load(image_path.c_str(), &image.width, &image.height,
                                &image.channels, STBI_rgb_alpha);
    if (image.channels != STBI_rgb_alpha) {
        image.channels = STBI_rgb_alpha;
    }
}

/*
 * Build a string with the image path given by the parent folder, resolution and image extention
 */
std::string
Image::format_image_path(const char *folder_path, const std::string& resolution_type, const std::string &extension_type) {
    auto filename = std::format("{}{}.{}", folder_path, resolution_type, extension_type);
    return filename;
}

/*
 * Load jpg images named i.jpg where i is a natural number.
 * All the images are put in a vector of Image struct. AoS.
 */
std::vector<Image> Image::load_images(int times, const std::string& filename) {
    std::vector<Image> img_lst;
    img_lst.reserve(times);
#pragma omp parallel for
    for (unsigned long i = 0; i < times; i++) {
        Image img;
        Image::loadImage(filename, img);
        if (!img.rgb_image) {
            std::cout << "ERROR: Failed to load image: " << filename << std::endl;
        } else {
#pragma omp critical
            img_lst.push_back(img);
        }
    }
    return img_lst;
}

/*
 * Saves image as png in a desired location.
 */
void Image::saveImage(const std::string &output_path, const Image image) {
    stbi_write_png(output_path.c_str(), image.width, image.height, image.channels,
                   image.rgb_image, image.width * image.channels);
}

