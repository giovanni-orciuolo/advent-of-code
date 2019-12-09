#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

// Actual code

int main() {
    std::ifstream image("bios_password.sif");
    std::string image_data; getline(image, image_data);


    const int IMAGE_WIDTH = 25, IMAGE_HEIGHT = 6;
    const int LAYER_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;

    std::vector<std::string> layers{};

    for (int i = 0; i < image_data.length(); ++i) {
        if (i % LAYER_SIZE == 0) {
            layers.push_back(image_data.substr(i, LAYER_SIZE));
        }
    }

    int min_zeroes = LAYER_SIZE + 1;
    std::string best_layer{};

    for (auto layer : layers) {
        int zeroes = std::count(layer.begin(), layer.end(), '0');
        if (zeroes < min_zeroes) {
            min_zeroes = zeroes;
            best_layer = layer;
        }
    }

    int ones = std::count(best_layer.begin(), best_layer.end(), '1');
    int twos = std::count(best_layer.begin(), best_layer.end(), '2');

    std::cout << "Answer for part 1: " << ones * twos << std::endl; 

    // PART 2: Decoding the image

    std::string decoded_image(LAYER_SIZE, ' ');

    for (int pixel = 0; pixel < LAYER_SIZE; ++pixel) {
        for (auto layer : layers) {
            if (layer[pixel] != '2') {
                decoded_image[pixel] = layer[pixel];
                break;
            }
        }
    }

    // Render the image
    int i = 0;
    for (int row = 0; row < IMAGE_HEIGHT; ++row) {
        for (int col = 0; col < IMAGE_WIDTH; ++col) {
            switch (decoded_image[i]) {
                case '0': std::cout << "▓"; break;
                case '1': std::cout << "░"; break;
                case '2': std::cout << " "; break;
            }
            i++;
        }
        std::cout << std::endl;
    }

    return 0;
}