#pragma once
#include <bit>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
namespace SF::DDSLoader {
struct DDSData {
  uint32_t width, height;
  std::vector<uint8_t> data;
};
struct Color {
  float r, g, b, a;
};

Color fromRGB565(unsigned short rgb565) {
  Color color;
  color.r = ((rgb565 >> 11) & 0x1F) * 1.0f / 31.0f;
  color.g = ((rgb565 >> 5) & 0x3F) * 1.0f / 63.0f;
  color.b = (rgb565 & 0x1F) * 1.0f / 31.0f;
  color.a = 1.0f;
  return color;
};

Color Lerp(Color x, Color y, float s) {
  Color result;
  result.r = x.r + s * (y.r - x.r);
  result.g = x.g + s * (y.g - x.g);
  result.b = x.b + s * (y.b - x.b);
  result.a = x.a + s * (y.a - x.a);
  return result;
}

DDSData load(const std::filesystem::path& path) {
  // simple dds loader
  // see
  // https://techblog.sega.jp/entry/2016/12/26/100000#BC1%E5%BD%A2%E5%BC%8F%E3%81%AB%E3%82%82%E5%AF%BE%E5%BF%9C%E3%81%99%E3%82%8B
  // todo: read formal spec and implement all sub-versions
  // I think current implementation is only for DXT1

  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }
  DDSData dds_data{};

  const int header_size = 128;
  std::vector<uint8_t> header(header_size);
  file.read(reinterpret_cast<char*>(header.data()), header_size);

  dds_data.height = *reinterpret_cast<uint32_t*>(&header[12]);
  dds_data.width = *reinterpret_cast<uint32_t*>(&header[16]);

  std::vector<uint8_t> type(4);
  std::copy(header.begin() + (5 * 16 + 4), header.begin() + (5 * 16 + 8),
            type.begin());

  bool is_dds = header[0] == 'D' && header[1] == 'D' && header[2] == 'S' &&
                header[3] == ' ';
  if (!is_dds) {
    std::cout << "Not a DDS file" << std::endl;
    return dds_data;
  }

  bool is_dxt1 =
      type[0] == 0x44 && type[1] == 0x58 && type[2] == 0x54 && type[3] == 0x31;

  //   std::cout << "Width: " << dds_data.width << std::endl;
  //   std::cout << "Height: " << dds_data.height << std::endl;
  //   std::cout << "is_dxt1: " << (is_dxt1 ? "true" : "false") << std::endl;

  if (!is_dxt1) {
    std::cout << "Not a DXT1 file" << std::endl;
    return dds_data;
  }

  auto block_count = ((dds_data.width + 3) / 4) * ((dds_data.height + 3) / 4);
  std::cout << "Block count: " << block_count << std::endl;

  std::vector<std::vector<Color>> bitmap(dds_data.height,
                                         std::vector<Color>(dds_data.width));

  uint32_t row = (dds_data.width + 3) / 4;
  uint32_t col = (dds_data.height + 3) / 4;
  for (size_t i = 0; i < row; i++) {
    for (size_t j = 0; j < col; j++) {
      std::vector<uint8_t> block(8);
      file.read(reinterpret_cast<char*>(block.data()), 8);

      Color color0 = fromRGB565(*reinterpret_cast<unsigned short*>(&block[0]));
      Color color1 = fromRGB565(*reinterpret_cast<unsigned short*>(&block[2]));

      auto c0 = *reinterpret_cast<uint16_t*>(&block[0]);
      auto c1 = *reinterpret_cast<uint16_t*>(&block[2]);

      auto color_table = std::vector<Color>(4);
      color_table[0] = color0;
      color_table[1] = color1;
      if (c0 > c1) {
        color_table[2] = Lerp(color0, color1, 1.0f / 3.0f);
        color_table[3] = Lerp(color0, color1, 2.0f / 3.0f);
      } else {
        color_table[2] = Lerp(color0, color1, 0.5f);
        color_table[3] = Color({0, 0, 0, 1});
      }

      auto index_bits = *reinterpret_cast<uint32_t*>(&block[4]);
      for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
          auto idx = index_bits & 0x03;

          int xx = i * 4 + x;
          int yy = j * 4 + y;
          if (xx >= dds_data.width || yy >= dds_data.height) {
            continue;
          }

          bitmap[yy][xx] = color_table[idx];
          index_bits >>= 2;
        }
      }
    }
  }

  file.close();

  auto output_file = std::ofstream("output.ppm");
  output_file << "P3" << std::endl;
  output_file << dds_data.width << " " << dds_data.height << std::endl;
  output_file << "255" << std::endl;

  for (int i = 0; i < dds_data.height; i++) {
    for (int j = 0; j < dds_data.width; j++) {
      auto color = bitmap[i][j];
      output_file << static_cast<int>(color.r * 255) << " "
                  << static_cast<int>(color.g * 255) << " "
                  << static_cast<int>(color.b * 255) << " ";
    }
    output_file << std::endl;
  }
  output_file.close();

  return dds_data;
}
}  // namespace SF::DDSLoader