#pragma once
#include <stdint.h>

#include <array>
#include <bitset>
#include <filesystem>
#include <vector>

namespace SF::DDSLoader {

enum class DDSFlags : uint32_t {
  caps = 0x00000001,
  height = 0x00000002,
  width = 0x00000004,
  pitch = 0x00000008,
  pixel_format = 0x00001000,
  mip_map_count = 0x00020000,
  linear_size = 0x00080000,
  depth = 0x00800000,
};

std::vector<std::string> dump_flags(uint32_t flags);

enum class DDSPixelFormatFlags {
  alpha_pixels = 0x00000001,
  alpha = 0x00000002,
  four_cc = 0x00000004,
  palette_indexed4 = 0x00000008,
  palette_indexed8 = 0x00000020,
  rgb = 0x00000040,
  luminance = 0x00020000,
  bump_dudv = 0x00080000,
};

enum class DDSCaps {
  alpha = 0x00000002,
  complex = 0x00000008,
  texture = 0x00001000,
  mip_map = 0x00400000,
};

enum class DDSCaps2 {
  cubemap = 0x00000200,
  cubemap_positive_x = 0x00000400,
  cubemap_negative_x = 0x00000800,
  cubemap_positive_y = 0x00001000,
  cubemap_negative_y = 0x00002000,
  cubemap_positive_z = 0x00004000,
  cubemap_negative_z = 0x00008000,
  volume = 0x00400000,
};

enum class FourCC {
  DXT1 = 0x44585431,  // DXT1
  DXT2 = 0x44585432,  // DXT2
  DXT3 = 0x44585433,  // DXT3
  DXT4 = 0x44585434,  // DXT4
  DXT5 = 0x44585435,  // DXT5
  DX10 = 0x30315844,  // DX10
  ATI1 = 0x31495441,  // ATI1
  A16B16G16R16 = 0x00000024,
  Q16W16V16U16 = 0x0000006E,
  R16F = 0x0000006F,
  G16R16F = 0x00000070,
  A16B16G16R16F = 0x00000071,
  R32F = 0x00000072,
  G32R32F = 0x00000073,
  A32B32G32R32F = 0x00000074,
  CxV8U8 = 0x00000075,
  Q8W8V8V8 = 0x0000003f,
};

std::string to_string(FourCC fourCC);

struct DDSHeader {
  uint8_t magic[4];
  uint32_t size;
  uint32_t flags;
  uint32_t height;
  uint32_t width;
  uint32_t pitch_or_linear_size;
  uint32_t depth;
  uint32_t mip_map_count;
  uint32_t reserved[11];
  struct {
    uint32_t size;
    uint32_t flags;
    FourCC fourCC;
    uint32_t rgb_bit_count;
    uint32_t r_bit_mask;
    uint32_t g_bit_mask;
    uint32_t b_bit_mask;
    uint32_t a_bit_mask;
  } pixel_format;
  uint32_t caps[4];
  uint32_t reserved2;
};

using DXGIFormat = uint32_t;

enum class DDSResourceDimension {
  unknown = 0,
  buffer = 1,
  texture1d = 2,
  texture2d = 3,
  texture3d = 4,
};

struct DDSHeaderDXT10 {
  DXGIFormat dxgi_format;
  DDSResourceDimension resource_dimension;
  uint32_t misc_flag;
  uint32_t array_size;
  uint32_t misc_flags2;
};

DDSHeader parse_dds_header(const std::array<uint8_t, sizeof(DDSHeader)>& data);

bool is_dx10(const DDSHeader& header);

DDSHeaderDXT10 parse_dds_header_dx10(
    const std::array<uint8_t, sizeof(DDSHeaderDXT10)>& data);

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

RGBA8888 Lerp(RGBA8888 x, RGBA8888 y, float s);

DDSData load(const std::filesystem::path& path);
}  // namespace SF::DDSLoader