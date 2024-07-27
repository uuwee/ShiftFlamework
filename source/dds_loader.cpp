#include "dds_loader.hpp"

#include <bit>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace SF::DDSLoader {

std::vector<std::string> dump_flags(uint32_t flags) {
  std::vector<std::string> result;
  if (flags & static_cast<uint32_t>(DDSFlags::caps)) {
    result.push_back("DDS_CAPS");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::height)) {
    result.push_back("DDS_HEIGHT");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::width)) {
    result.push_back("DDS_WIDTH");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::pitch)) {
    result.push_back("DDS_PITCH");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::pixel_format)) {
    result.push_back("DDS_PIXELFORMAT");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::mip_map_count)) {
    result.push_back("DDS_MIPMAPCOUNT");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::linear_size)) {
    result.push_back("DDS_LINEARSIZE");
  }
  if (flags & static_cast<uint32_t>(DDSFlags::depth)) {
    result.push_back("DDS_DEPTH");
  }
  return result;
}

std::string to_string(FourCC fourCC) {
  return std::string(reinterpret_cast<const char*>(&fourCC), 4);
}

uint32_t readUInt32LE(const uint8_t* data) {
  return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

DDSHeader parse_dds_header(const std::array<uint8_t, sizeof(DDSHeader)>& data) {
  DDSHeader header;
  std::memset(&header, 0, sizeof(header));

  auto magic = std::string(data.begin(), data.begin() + 4);
  if (magic != "DDS ") {
    std::cerr << "Not a DDS file" << std::endl;
    return header;
  }

  const uint8_t* ptr = data.data();

  header.magic[0] = *ptr++;
  header.magic[1] = *ptr++;
  header.magic[2] = *ptr++;
  header.magic[3] = *ptr++;

  header.size = readUInt32LE(ptr);
  ptr += 4;

  header.flags = readUInt32LE(ptr);
  ptr += 4;

  header.height = readUInt32LE(ptr);
  ptr += 4;

  header.width = readUInt32LE(ptr);
  ptr += 4;

  header.pitch_or_linear_size = readUInt32LE(ptr);
  ptr += 4;

  header.depth = readUInt32LE(ptr);
  ptr += 4;

  header.mip_map_count = readUInt32LE(ptr);
  ptr += 4;

  for (int i = 0; i < 11; ++i) {
    header.reserved[i] = readUInt32LE(ptr);
    ptr += 4;
  }

  header.pixel_format.size = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.flags = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.fourCC = static_cast<FourCC>(readUInt32LE(ptr));
  ptr += 4;

  header.pixel_format.rgb_bit_count = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.r_bit_mask = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.g_bit_mask = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.b_bit_mask = readUInt32LE(ptr);
  ptr += 4;

  header.pixel_format.a_bit_mask = readUInt32LE(ptr);
  ptr += 4;

  for (int i = 0; i < 4; ++i) {
    header.caps[i] = readUInt32LE(ptr);
    ptr += 4;
  }

  header.reserved2 = readUInt32LE(ptr);

  return header;
}

DDSHeaderDXT10 parse_dds_header_dx10(
    const std::array<uint8_t, sizeof(DDSHeaderDXT10)>& data) {
  return DDSHeaderDXT10();
}

std::string dump_raw_dds_header(DDSHeader header) { return ""; }

RGBA8888 fromRGB565(unsigned short rgb565) {
  RGBA8888 color;
  color.r = ((rgb565 >> 11) & 0x1F) * 1.0f / 31.0f * 255.0f;
  color.g = ((rgb565 >> 5) & 0x3F) * 1.0f / 63.0f * 255.0f;
  color.b = (rgb565 & 0x1F) * 1.0f / 31.0f * 255.0f;
  color.a = 255;
  return color;
}

RGBA8888 Lerp(RGBA8888 x, RGBA8888 y, float s) {
  RGBA8888 result;
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
  // std::cout << "loading dds file: " << path << std::endl;
  std::ifstream file(path, std::ios::binary);
  if (!file.is_open()) {
    return DDSData{
        .width = 0,
        .height = 0,
    };
  }
  DDSData dds_data{};

  DDSHeader raw_header;

  const int header_size = 128;
  std::vector<uint8_t> header(header_size);
  file.read(reinterpret_cast<char*>(header.data()), header_size);

  memcpy(&raw_header, header.data(), sizeof(DDSHeader));
  std::cout << dump_raw_dds_header(raw_header) << std::endl;

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
  bool is_dxt5 =
      type[0] == 0x44 && type[1] == 0x58 && type[2] == 0x54 && type[3] == 0x35;

  auto block_count = ((dds_data.width + 3) / 4) * ((dds_data.height + 3) / 4);
  std::cout << "Block count: " << block_count << std::endl;

  if ((!is_dxt1) && (!is_dxt5)) {
    std::cout << "Not a DXT1 nor DXT5 file" << std::endl;
    return dds_data;
  }

  std::vector<std::vector<RGBA8888>> bitmap(
      dds_data.height, std::vector<RGBA8888>(dds_data.width));

  uint32_t row = (dds_data.width + 3) / 4;
  uint32_t col = (dds_data.height + 3) / 4;
  for (size_t i = 0; i < row; i++) {
    for (size_t j = 0; j < col; j++) {
      if (is_dxt1) {
        std::vector<uint8_t> block(8);
        file.read(reinterpret_cast<char*>(block.data()), 8);

        RGBA8888 color0 = SF::DDSLoader::fromRGB565(
            *reinterpret_cast<unsigned short*>(&block[0]));
        RGBA8888 color1 = SF::DDSLoader::fromRGB565(
            *reinterpret_cast<unsigned short*>(&block[2]));

        auto c0 = *reinterpret_cast<uint16_t*>(&block[0]);
        auto c1 = *reinterpret_cast<uint16_t*>(&block[2]);

        auto color_table = std::vector<RGBA8888>(4);
        color_table[0] = color0;
        color_table[1] = color1;
        if (c0 > c1) {
          color_table[2] = SF::DDSLoader::Lerp(color0, color1, 1.0f / 3.0f);
          color_table[3] = SF::DDSLoader::Lerp(color0, color1, 2.0f / 3.0f);
        } else {
          color_table[2] = SF::DDSLoader::Lerp(color0, color1, 0.5f);
          color_table[3] = RGBA8888({0, 0, 0, 0});
        }
        auto index_bits = *reinterpret_cast<uint32_t*>(&block[4]);
        for (int y = 0; y < 4; y++) {
          for (int x = 0; x < 4; x++) {
            auto idx = index_bits & 0x03;  // see lower 2 bits

            int xx = j * 4 + x;
            int yy = i * 4 + y;
            if (xx >= dds_data.width || yy >= dds_data.height) {
              continue;
            }

            bitmap[yy][xx] = color_table[idx];
            index_bits >>= 2;  // see next lower 2 bits
          }
        }
      } else if (is_dxt5) {
        {
          std::vector<uint8_t> alpha_block(8);
          file.read(reinterpret_cast<char*>(alpha_block.data()), 8);

          auto a0 = alpha_block[0];
          auto a1 = alpha_block[1];

          auto alpha_table = std::vector<uint8_t>(8);
          alpha_table[0] = a0;
          alpha_table[1] = a1;

          if (a0 > a1) {
            alpha_table[2] = (6.0f * a0 + 1.0f * a1) / 7.0f;
            alpha_table[3] = (5.0f * a0 + 2.0f * a1) / 7.0f;
            alpha_table[4] = (4.0f * a0 + 3.0f * a1) / 7.0f;
            alpha_table[5] = (3.0f * a0 + 4.0f * a1) / 7.0f;
            alpha_table[6] = (2.0f * a0 + 5.0f * a1) / 7.0f;
            alpha_table[7] = (1.0f * a0 + 6.0f * a1) / 7.0f;
          } else {
            alpha_table[2] = (4.0f * a0 + 1.0f * a1) / 5.0f;
            alpha_table[3] = (3.0f * a0 + 2.0f * a1) / 5.0f;
            alpha_table[4] = (2.0f * a0 + 3.0f * a1) / 5.0f;
            alpha_table[5] = (1.0f * a0 + 4.0f * a1) / 5.0f;
            alpha_table[6] = 0;
            alpha_table[7] = 255;
          }
          auto index_bits = std::vector<uint8_t>(6);
          std::copy(alpha_block.begin() + 2, alpha_block.end(),
                    index_bits.begin());
          for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
              auto idx = (int)(*index_bits.data() & 0x07);

              int xx = j * 4 + x;
              int yy = i * 4 + y;
              if (xx >= dds_data.width || yy >= dds_data.height) {
                continue;
              }

              bitmap[yy][xx].a = alpha_table[idx];
              *index_bits.data() >>= 3;
            }
          }
        }
        {
          std::vector<uint8_t> col_block(8);
          file.read(reinterpret_cast<char*>(col_block.data()), 8);

          RGBA8888 color0 = SF::DDSLoader::fromRGB565(
              *reinterpret_cast<unsigned short*>(&col_block[0]));
          RGBA8888 color1 = SF::DDSLoader::fromRGB565(
              *reinterpret_cast<unsigned short*>(&col_block[2]));

          auto c0 = *reinterpret_cast<uint16_t*>(&col_block[0]);
          auto c1 = *reinterpret_cast<uint16_t*>(&col_block[2]);

          auto color_table = std::vector<RGBA8888>(4);
          color_table[0] = color0;
          color_table[1] = color1;
          if (c0 > c1) {
            color_table[2] = SF::DDSLoader::Lerp(color0, color1, 1.0f / 3.0f);
            color_table[3] = SF::DDSLoader::Lerp(color0, color1, 2.0f / 3.0f);
          } else {
            color_table[2] = SF::DDSLoader::Lerp(color0, color1, 0.5f);
            color_table[3] = RGBA8888({0, 0, 0, 0});
          }
          auto index_bits = *reinterpret_cast<uint32_t*>(&col_block[4]);
          for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
              auto idx = index_bits & 0x03;

              int xx = j * 4 + x;
              int yy = i * 4 + y;
              if (xx >= dds_data.width || yy >= dds_data.height) {
                continue;
              }

              bitmap[yy][xx].r = color_table[idx].r;
              bitmap[yy][xx].g = color_table[idx].g;
              bitmap[yy][xx].b = color_table[idx].b;

              index_bits >>= 2;
            }
          }
        }
      }
    }
  }

  file.close();

  dds_data.data = std::vector<RGBA8888>(dds_data.width * dds_data.height);
  for (int i = 0; i < dds_data.height; i++) {
    for (int j = 0; j < dds_data.width; j++) {
      dds_data.data[i * dds_data.width + j] = bitmap[i][j];
    }
  }

  dds_data.alpha = is_dxt5;

  // auto output_file = std::ofstream("output.ppm");
  // output_file << "P3" << std::endl;
  // output_file << dds_data.width << " " << dds_data.height << std::endl;
  // output_file << "255" << std::endl;

  // for (int i = 0; i < dds_data.height; i++) {
  //   for (int j = 0; j < dds_data.width; j++) {
  //     auto color = bitmap[i][j];
  //     output_file << static_cast<int>(color.r) << " "
  //                 << static_cast<int>(color.g) << " "
  //                 << static_cast<int>(color.b) << " ";
  //   }
  //   output_file << std::endl;
  // }
  // output_file.close();
  return dds_data;
}
}  // namespace SF::DDSLoader