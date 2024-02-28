#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <omp.h>

#define FOREGROUND_PATH "../Input/kitty.png"
#define BACKGROUND_PATH "../Input/Backgrounds/"
#define OUTPUT_PATH "../Output/"

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
    Image foreground;
    loadImage(FOREGROUND_PATH,foreground);

    double totalStartTime = omp_get_wtime();

    double startTime = omp_get_wtime();
    auto backgrounds = load_images(1, maxImageIdx);
    double endTime = omp_get_wtime();
    std::cout << "Backgrounds loaded #: " << backgrounds.size() << " - expected #: " << maxImageIdx << std::endl;
    std::cout << "Loading time: " << endTime - startTime << std::endl << std::endl;

    std::cout << "Starting alpha-composing" << std::endl;
    startTime = omp_get_wtime();
    bool isComposed;
    for(Image &background : backgrounds){
        isComposed = compose(foreground, background);
        if(!isComposed){
            std::cout << "Foreground is bigger than backgroud: "
            << foreground.height<<"x"<<foreground.width << " vs "
            << background.height<<"x"<<background.width << std::endl;
        }
    }
    endTime = omp_get_wtime();
    std::cout << "Compositing time: " << endTime - startTime << std::endl << std::endl;

    startTime = omp_get_wtime();
    std::cout << "Saving output image in "<< OUTPUT_PATH << std::endl;

    for(int i = 0; i < backgrounds.size(); ++i){
        char filename[32];
        sprintf(filename, "%s%i.png", OUTPUT_PATH,i);
        saveImage(filename,backgrounds[i]);
    }
    endTime = omp_get_wtime();
    std::cout << "Output image saved" << std::endl;
    std::cout << "Saving time: " << endTime - startTime << std::endl << std::endl;

    startTime = omp_get_wtime();
    std::cout << "Releasing memory resources" << std::endl;
    stbi_image_free(foreground.rgb_image);
    for(Image &background : backgrounds){
        stbi_image_free(background.rgb_image);
    }
    backgrounds.clear();
    endTime = omp_get_wtime();
    std::cout << "Memory realeased" << std::endl;
    std::cout << "Realising time: " << endTime - startTime << std::endl << std::endl;

    double totalEndTime = omp_get_wtime();
    std::cout << "Total run time: " << totalEndTime - totalStartTime << std::endl << std::endl;
    return 0;
}
