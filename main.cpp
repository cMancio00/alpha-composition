#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <omp.h>

#define FOREGROUND_PATH "../Input/kitty.png"
#define BACKGROUND_PATH "../Input/Backgrounds/"
#define OUTPUT_PATH "../Output/output.png"

static const int maxImageIdx = 564;

struct Image{
    int width{0},height{0},channels{0};
    uint8_t *rgb_image{nullptr};
};

/*
 * Load a single image. If image is not RGBA, alpha channel is assumed completely opaque (255).
 */
void loadImage(const std::string &image_path, Image &image){
    image.rgb_image = stbi_load(image_path.c_str(),&image.width, &image.height,
                                     &image.channels, STBI_rgb_alpha);
    if(image.channels != STBI_rgb_alpha){
        image.channels = STBI_rgb_alpha;
    }
}
/*
 * Load jpg images named i.jpg where i is a natural number.
 * All the images are put in a vector of Image struct. AoS.
 */
std::vector<Image> load_images(int start_idx, int end_idx) {
    std::vector<Image> img_lst;
    for (unsigned long i = start_idx; i <= end_idx; i++) {
        char filename[32];
        sprintf(filename, "%s%lu.jpg", BACKGROUND_PATH,i);
        Image img;
        loadImage(filename,img);
        if (!img.rgb_image) {
            std::cout << "ERROR: Failed to load image: " << filename << std::endl;
        } else {
            img_lst.push_back(img);
        }
    }
    return img_lst;
}

/*
 * Saves image as png in a desired location.
 */
void saveImage(const std::string &output_path, const Image image){
    stbi_write_png(output_path.c_str(), image.width, image.height, image.channels,
                   image.rgb_image, image.width * image.channels );
}

/*
 * Alpha-compose foreground image on background image.
 * Composition will be saved on background, while foreground remains untouched.
 */
void compose(const Image &foreground, Image &background) {
    for(int y = 0; y < foreground.height; ++y){
        for(int x = 0; x < foreground.width; ++x){

            int backgroundIndex = (y * background.width + x) * STBI_rgb_alpha;
            int foregroundIndex = (y * foreground.width + x) * STBI_rgb_alpha;

            float alpha = foreground.rgb_image[foregroundIndex + 3] / 255.0f;
            float beta = 1.0f - alpha;

            for (int color = 0; color < 3; ++color) {
                background.rgb_image[backgroundIndex + color] =
                        background.rgb_image[backgroundIndex + color] * beta
                        + foreground.rgb_image[foregroundIndex + color] * alpha;

            }
        }
    }
}

int main(){
    Image foreground;
    loadImage("../Input/Backgrounds/1.jpg",foreground);

    Image background;
    loadImage("../Input/1.jpg",background);

    double startTime = omp_get_wtime();
    auto backgrounds = load_images(1, maxImageIdx);
    double endTime = omp_get_wtime();
    std::cout << "Backgrounds loaded #: " << backgrounds.size() << " - expected #: " << maxImageIdx << std::endl;
    std::cout << "Loading time: " << endTime - startTime << std::endl << std::endl;

    startTime = omp_get_wtime();
    compose(foreground, background);
    endTime = omp_get_wtime();
    std::cout << "Compositing time: " << endTime - startTime << std::endl << std::endl;

    std::cout << "Saving output image as "<< OUTPUT_PATH << std::endl;
    saveImage(OUTPUT_PATH,background);
    std::cout << "Output image saved" << std::endl;

    std::cout << "Releasing memory resources" << std::endl;
    stbi_image_free(foreground.rgb_image);
    stbi_image_free(background.rgb_image);
    std::cout << "Done" << std::endl;

    return 0;
}
