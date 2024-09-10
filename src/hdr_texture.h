#ifndef HDR_TEXTURE_H
#define HDR_TEXTURE_H

#include "vec3.h"
#include "rtw_stb_image.h"
#include <string>
#include <iostream>
#include <cmath>

// 自定义的 clamp 函数
template <typename T>
T clamp(T value, T min_val, T max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

class hdr_texture {
public:
    // 构造函数，读取 HDR 图像
    hdr_texture(const std::string& filename) {
        int components_per_pixel = 3;
        data = stbi_loadf(filename.c_str(), &width, &height, &components_per_pixel, components_per_pixel);
        if (!data) {
            std::cerr << "ERROR: Could not load HDR image: " << filename << std::endl;
            data = nullptr;
        }
    }

    // 析构函数，释放图像数据
    ~hdr_texture() {
        if (data) {
            stbi_image_free(data);
        }
    }

    // 获取颜色值，根据 UV 坐标
    color value(double u, double v) const {
        if (!data) return color(0, 0, 0);

        // Flip the V coordinate (previously you had v = 1 - v)
        // v = 1.0 - v;  // Flip vertically

        // Wrap UV coordinates
        u = u - floor(u);
        v = v - floor(v);

        // Convert UV to pixel coordinates
        int i = static_cast<int>(u * width);
        int j = static_cast<int>(v * height);  // No need to flip `j` here, because `v` has already been flipped

        i = clamp(i, 0, width - 1);
        j = clamp(j, 0, height - 1);

        int index = (j * width + i) * 3;
        return color(data[index], data[index + 1], data[index + 2]);
    }


private:
    int width, height;
    float* data;
};

#endif
