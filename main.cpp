#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>

#define FOREGROUND_PATH "../Input/Background.png"
#define RGBA 4

struct Image{
    int width{0},height{0},channels{0};
    uint8_t *rgb_image{nullptr};
};

void loadImage(const std::string &image_path, Image &image){
    image.rgb_image = stbi_load(FOREGROUND_PATH,&image.width, &image.height,
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

int main(){
    Image foreground;
    loadImage(FOREGROUND_PATH,foreground);
    return 0;
}
