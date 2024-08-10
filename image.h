#ifndef ALPHA_COMPOSITION_IMAGE_H
#define ALPHA_COMPOSITION_IMAGE_H
#include <string>
#include <vector>
#include <cstdint>

struct Image {
    int width{0}, height{0}, channels{0};
    uint8_t *rgb_image{nullptr};

    static void loadImage(const std::string &image_path, Image &image);
    static std::vector<Image> load_images(int times, const std::string& filename);
    static void saveImage(const std::string &output_path, Image image);
    static std::string format_image_path(const char *folder_path,
                                  const std::string& resolution_type,
                                  const std::string &extension_type);

};




#endif //ALPHA_COMPOSITION_IMAGE_H
