#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <omp.h>
#include <format>

const char * FOREGROUND_PATH = "../Input/Foregrounds/";
const char * BACKGROUND_PATH = "../Input/Backgrounds/";
const char * OUTPUT_PATH = "../Output/";

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

std::string format_image_path(const char *folder_path, const std::string resolution_type, const std::string& extention_type){
    auto filename = std::format("{}{}.{}", folder_path, resolution_type,extention_type);
    return filename;
}

/*
 * Load jpg images named i.jpg where i is a natural number.
 * All the images are put in a vector of Image struct. AoS.
 */
std::vector<Image> load_images(int times,const std::string filename) {
    std::vector<Image> img_lst;
    img_lst.reserve(times);
    for (unsigned long i = 0; i < times; i++) {
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
bool compose(const Image &foreground, Image &background) {
    if(foreground.height > background.height | foreground.width > background.width){
        return false;
    }

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
    return true;
}

int main(){

/*
 * TODO: Format output strings to get a table of times
 */
    static const int times = 2;
    std::vector<std::string> resolutions = {"HD","FullHD","2K","4K"};

    for(short i = 0; i <= 2; ++i) {

        auto foreground_resolution = resolutions[i];
        auto background_resolution = resolutions[i+1];

        std::cout << std::format("Starting with Background resolution {} and Foreground resolution {}\n",
                                 background_resolution,foreground_resolution);
        std::cout << std::format("Loading images {} times.\n",times);

        auto foreground_path = format_image_path(FOREGROUND_PATH, foreground_resolution, "png");
        auto background_name = format_image_path(BACKGROUND_PATH, background_resolution, "jpg");
        auto output_name = format_image_path(OUTPUT_PATH, background_resolution, "png");

        //Load foreground
        Image foreground;
        loadImage(foreground_path, foreground);

        //Load Background images
        double totalStartTime = omp_get_wtime();
        double startTime = omp_get_wtime();
        auto backgrounds = load_images(times, background_name);
        double endTime = omp_get_wtime();
        std::cout << "Loading time: " << endTime - startTime << std::endl << std::endl;

        //Alpha compositing
        std::cout << "Starting alpha-composing" << std::endl;
        startTime = omp_get_wtime();
        bool isComposed;
        for (Image &background: backgrounds) {
            isComposed = compose(foreground, background);
            if (!isComposed) {
                std::cout << "Foreground is bigger than background: "
                          << foreground.height << "x" << foreground.width << " vs "
                          << background.height << "x" << background.width << std::endl;
            }
        }
        endTime = omp_get_wtime();
        std::cout << "Compositing time: " << endTime - startTime << std::endl << std::endl;


        //Saving the Composed image
        saveImage(output_name, backgrounds[0]);

        //Releasing Resources
        stbi_image_free(foreground.rgb_image);
        for (Image &background: backgrounds) {
            stbi_image_free(background.rgb_image);
        }
        backgrounds.clear();

        double totalEndTime = omp_get_wtime();
        std::cout << "Total run time: " << totalEndTime - totalStartTime << std::endl << std::endl;
    }
    return 0;
}
