#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <omp.h>

#define FOREGROUND_PATH "../Input/pooh2k.png"
#define BACKGROUND_PATH "../Input/1.jpg"
#define OUTPUT_PATH "../Output/output.png"

struct Image{
    int width{0},height{0},channels{0};
    uint8_t *rgb_image{nullptr};
};

void loadImage(const std::string &image_path, Image &image){
    image.rgb_image = stbi_load(image_path.c_str(),&image.width, &image.height,
                                     &image.channels, STBI_rgb_alpha);
    if(!image.rgb_image){
        std::cerr << "Unable to load image." << std::endl;
    }else{
        std::cout << "Image loaded." << std::endl;
        std::cout << image.width<<"x"<< image.height<< " and "
                  << image.channels << " channels." <<std::endl;
        if(image.channels != STBI_rgb_alpha){
            std::cout << "WARNING: Image does not have alpha channel!" << std::endl
            << "Forcing alpha channel to 255 (completely visible)." << std::endl;
            image.channels = STBI_rgb_alpha;
        }
    }
}

void saveImage(const std::string &output_path, const Image image){
    stbi_write_png(output_path.c_str(), image.width, image.height, image.channels,
                   image.rgb_image, image.width * image.channels );
}

int main(){
    Image foreground;
    loadImage(FOREGROUND_PATH,foreground);
    Image background;
    loadImage(BACKGROUND_PATH,background);

    double startTime = omp_get_wtime();

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

    double endTime = omp_get_wtime();
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
