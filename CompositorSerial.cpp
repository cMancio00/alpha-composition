#include "image.h"
#include <iostream>
#include <vector>
#include <omp.h>
#include <format>
#include <fstream>
#include "stb_image.h"

const char *FOREGROUND_PATH = "../Input/Foregrounds/";
const char *BACKGROUND_PATH = "../Input/Backgrounds/";
const char *OUTPUT_PATH = "../Output/";

/*
 * Alpha-compose foreground image on background image.
 * Composition will be saved on background, while foreground remains untouched.
 */
bool compose(const Image &foreground, Image &background) {
    if (foreground.height > background.height | foreground.width > background.width) {
        return false;
    }
#pragma omp parallel for collapse(2)
    for (int y = 0; y < foreground.height; ++y) {
        for (int x = 0; x < foreground.width; ++x) {

            int backgroundIndex = (y * background.width + x) * STBI_rgb_alpha;
            int foregroundIndex = (y * foreground.width + x) * STBI_rgb_alpha;

            float alpha = foreground.rgb_image[foregroundIndex + 3] / 255.0f;
            float beta = 1.0f - alpha;
#pragma omp simd
            for (int color = 0; color < 3; ++color) {
                background.rgb_image[backgroundIndex + color] =
                        background.rgb_image[backgroundIndex + color] * beta
                        + foreground.rgb_image[foregroundIndex + color] * alpha;

            }
        }
    }
    return true;
}

int main() {

    std::vector<int> threads = {1, 25, 50, 100, 200, 500};
    std::vector<int> times_vector = {100, 250, 500};
    std::vector<std::string> resolutions = {"HD", "FullHD", "2K", "4K"};
    std::ofstream csv;
    csv.open ("../output.csv");
    csv << "threads,images,background,foreground,composing\n";

    for (short i = 0; i <= 2; ++i) {

        auto foreground_resolution = resolutions[i];
        auto background_resolution = resolutions[i + 1];

        auto foreground_path = Image::format_image_path(FOREGROUND_PATH, foreground_resolution, "png");
        auto background_name = Image::format_image_path(BACKGROUND_PATH, background_resolution, "jpg");
        auto output_name = Image::format_image_path(OUTPUT_PATH, background_resolution, "png");

        for (int times: times_vector) {
            for (int num_threads: threads) {
                omp_set_num_threads(num_threads);

                std::cout << std::format("{} over {}, {} images, {} threads\n",
                                         foreground_resolution,background_resolution,times,num_threads);

                double totalStartTime = omp_get_wtime();
                //Load foreground
                Image foreground;
                Image::loadImage(foreground_path, foreground);

                //Load Background images
                auto backgrounds = Image::load_images(times, background_name);

                //Alpha compositing
                double startTime = omp_get_wtime();
                bool isComposed;
                for (Image &background: backgrounds) {
                    isComposed = compose(foreground, background);
                    if (!isComposed) {
                        std::cout << "Foreground is bigger than background: "
                                  << foreground.height << "x" << foreground.width << " vs "
                                  << background.height << "x" << background.width << std::endl;
                    }
                }
                double endTime = omp_get_wtime();
                double composing = endTime - startTime;
                std::string composing_time = std::format("Compositing time: {:.4f}", composing);

                //Saving the Composed image
                Image::saveImage(output_name, backgrounds[0]);

                //Releasing Resources
                stbi_image_free(foreground.rgb_image);
                for (Image &background: backgrounds) {
                    stbi_image_free(background.rgb_image);
                }
                backgrounds.clear();

                double totalEndTime = omp_get_wtime();
                double total = totalEndTime - totalStartTime;
                std::string total_time = std::format("Total run time: {:.4f}", total);

                std::cout << std::format("{}\n{}\n",composing_time,total_time);

                csv << std::format("{},{},{},{},{:.4f},{:.4f}\n",
                                   num_threads,times,background_resolution,foreground_resolution,
                                   composing,total);
            }
        }
    }
    csv.close();
    return 0;
}
