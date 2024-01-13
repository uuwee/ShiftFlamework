#include "screenspace_mesh.hpp"

#include "engine.hpp"
#include "screenspace_mesh_renderer.hpp"

using namespace ShiftFlamework;

void ScreenSpaceMesh::on_register() {
  vertices = {
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, -0.5}),
                        .texture_coord = Math::Vector2f({0, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, -0.5}),
                        .texture_coord = Math::Vector2f({1, 0})},
      ScreenSpaceVertex{.position = Math::Vector2f({+0.5, +0.5}),
                        .texture_coord = Math::Vector2f({1, 1})},
      ScreenSpaceVertex{.position = Math::Vector2f({-0.5, +0.5}),
                        .texture_coord = Math::Vector2f({0, 1})},
  };

  indices = {0, 1, 2, 0, 2, 3};
  create_gpu_buffer();
  Engine::get_module<ScreenSpaceMeshRenderer>()->register_mesh(
      get_entity()->get_component<ScreenSpaceMesh>());
}

void ScreenSpaceMesh::create_gpu_buffer() {
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "vertex buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
        .size = vertices.size() * sizeof(ScreenSpaceVertex),
        .mappedAtCreation = false};

    vertex_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(vertex_buffer, vertices);
  }
  {
    const wgpu::BufferDescriptor buffer_desc{
        .nextInChain = nullptr,
        .label = "index buffer",
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index,
        .size = indices.size() * sizeof(uint32_t),
        .mappedAtCreation = false,
    };
    index_buffer = Engine::get_module<Graphics>()->create_buffer(buffer_desc);
    Engine::get_module<Graphics>()->update_buffer(index_buffer, indices);
  }
}

const std::vector<ScreenSpaceVertex> ScreenSpaceMesh::get_vertices() {
  return vertices;
}

const std::vector<uint32_t> ScreenSpaceMesh::get_indices() { return indices; }
const wgpu::Buffer ScreenSpaceMesh::get_vertex_buffer() {
  return vertex_buffer;
}
const wgpu::Buffer ScreenSpaceMesh::get_index_buffer() { return index_buffer; }

EXPORT void* ShiftFlamework_Entity_get_component_ShiftFlamework_ScreenSpaceMesh(
    void* self) {
  auto entity = std::shared_ptr<ShiftFlamework::Entity>(
      (ShiftFlamework::Entity*)self,
      [&](ShiftFlamework::Entity* ptr) { ptr->remove_reference(); });
  auto component = entity->add_component<ScreenSpaceMesh>();
  //auto component = ((ShiftFlamework::Entity*) self)->get_component<ScreenSpaceMesh>();

  component->add_reference();
  return component.get();
}

EXPORT void* ShiftFlamework_Entity_add_component_ShiftFlamework_ScreenSpaceMesh(
    void* self) {
  auto component = ((ShiftFlamework::Entity*)self)->add_component<ScreenSpaceMesh>();
  component->add_reference();
  return component.get();
}
