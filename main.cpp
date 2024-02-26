#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>

#define FOREGROUND_PATH "../Input/kitty.jpg"
#define BACKGROUND_PATH "../Input/pooh.png"

#define RGB 3
#define RGBA 4
#define OUTPUT_PATH "../Output/output.png"

struct Image{
    int width{0},height{0},channels{0};
    uint8_t *rgb_image{nullptr};
};

void loadImage(const std::string &image_path, Image &image){
    //TODO: usare stbi_info per leggere il numero di canali
    image.rgb_image = stbi_load(image_path.c_str(),&image.width, &image.height,
                                     &image.channels, RGBA);
    if(!image.rgb_image){
        std::cerr << "Unable to load image." << std::endl;
    }else{
        std::cout << "Image loaded." << std::endl;
        std::cout << image.width<<"x"<< image.height<< " and "
                  << image.channels << " channels." <<std::endl;
        if(image.channels != RGBA){
            std::cout << "WARNING: Image does not have alpha channel!" << std::endl;
        }
    }
}

void saveImage(const std::string &output_path, const Image image){
    stbi_write_png(output_path.c_str(), image.width, image.height, image.channels,
                   image.rgb_image, image.width * image.channels);
}

int main(){
    Image foreground;
    loadImage(FOREGROUND_PATH,foreground);
    Image background;
    loadImage(BACKGROUND_PATH,background);


    for(int y = 0; y < foreground.height; ++y){
        for(int x = 0; x < foreground.width; ++x){
            //Making index to
            int backgroundIndex = (y * background.width + x) * background.channels;
            for (int c = 0; c < RGBA; ++c) {
                background.rgb_image[backgroundIndex + c] = 255;
            }
        }
    }


    std::cout << "Done" << std::endl;

    saveImage(OUTPUT_PATH,background);
    std::cout << "Saved" << std::endl;

    stbi_image_free(foreground.rgb_image);
    stbi_image_free(background.rgb_image);



    return 0;
}
