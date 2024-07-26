#pragma once
#include <stdint.h>

#include <filesystem>
#include <vector>
#include <array>
#include <bitset>

namespace SF::DDSLoader {
struct DDSHeader{
  uint32_t size;
  std::bitset<32> flags;
  uint32_t height;
  uint32_t width;
  uint32_t pitch_or_linear_size;
  uint32_t depth;
  uint32_t mip_map_count;
  uint32_t reserved[11];
  struct {
    uint32_t size;
    uint32_t flags;
    uint32_t fourCC;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
  } pixel_format;
  uint32_t caps[4];
  uint32_t reserved2;
};

DDSHeader parse_dds_header(const std::array<uint8_t, 128>& data);

std::string dump_raw_dds_header(DDSHeader header);

struct RGBA8888 {
  uint8_t r, g, b, a;
};
struct DDSData {
  uint32_t width, height;
  std::vector<RGBA8888> data;
  bool alpha = false;
};

RGBA8888 fromRGB565(unsigned short rgb565);

RGBA8888 Lerp(RGBA8888 x, RGBA8888 y, float s) ;

DDSData load(const std::filesystem::path& path);
}  // namespace SF::DDSLoader