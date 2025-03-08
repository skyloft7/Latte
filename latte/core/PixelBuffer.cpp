#include "PixelBuffer.h"
#include "glm.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stb/stb_image_write.h"

void PixelBuffer::setPixel(int x, int y, glm::vec4 color) {
    mVector.insert(mVector.begin() + (y * mWidth) + x, color);
}

glm::vec4 PixelBuffer::getPixel(int x, int y) {
    return mVector[y * mWidth + x];
}


void PixelBuffer::exportImage(std::string path) {

    std::vector<uint8_t> image;

    for (int y = 0; y < mHeight; y++) {
        for (int x = 0; x < mWidth; x++) {
            glm::vec4 pixel = getPixel(x, y);

            float r = glm::clamp(pixel.r, 0.0f, 1.0f);
            float g = glm::clamp(pixel.g, 0.0f, 1.0f);
            float b = glm::clamp(pixel.b, 0.0f, 1.0f);

            image.push_back(r * 255);
            image.push_back(g * 255);
            image.push_back(b * 255);



        }
    }

    stbi_write_png(path.c_str(), mWidth, mHeight, 3, &image[0], mWidth * 3);

}
