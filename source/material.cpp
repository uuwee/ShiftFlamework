#include "material.hpp"

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"
using namespace ShiftFlamework;

const Math::Vector2f Material::get_uv_offset() { return uv_offset; }

const Math::Vector2f Material::get_tile_scale() { return tile_scale; }