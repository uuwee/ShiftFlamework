#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <windows.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>
#include <vector>

#include "dds_loader.hpp"
#include "engine.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "reflection_renderer.hpp"
#include "screenspace_material.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_mesh_renderer.hpp"
#include "script.hpp"
#include "test_image.h"
#include "transform.hpp"
#include "vector.hpp"
#include "window.hpp"
using namespace SF;

void import() {
  const auto filePath =
      "D:/resources/models/Bistro_v5_2/Bistro_v5_2/BistroExterior.fbx";
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(
      filePath, aiProcess_Triangulate | aiProcess_CalcTangentSpace |
                    aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
  if (scene == nullptr) {
    std::cerr << "failed to load: " << filePath
              << "assimp error: " << importer.GetErrorString() << std::endl;
  } else {
    aiMatrix4x4 t;
    t.a1 = 1;
    t.b2 = 1;
    t.c3 = 1;
    t.d4 = 1;

    // search_node(scene, scene->mRootNode, t);

    for (int child = 0; child < scene->mRootNode->mChildren[0]->mNumChildren;
         child++) {
      auto node = scene->mRootNode->mChildren[0]->mChildren[child];
      // std::cout << "name=" << node->mName.C_Str()
      //           << " #children=" << node->mNumChildren
      //           << " #meshes=" << node->mNumMeshes << std::endl;
      if (node->mNumChildren > 1) {
        // in bistro scene, this case is camera or directional light
        continue;
      }

      for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh_index = node->mMeshes[i];
        auto mesh = scene->mMeshes[mesh_index];
        // std::cout << "  mesh=" << mesh->mName.C_Str() << std::endl;
        // std::cout << "  num vertices=" << mesh->mNumVertices << std::endl;
        // std::cout << "  num faces=" << mesh->mNumFaces << std::endl;

        auto e = Engine::get_module<EntityStore>()->create();

        // transform
        e->add_component<Transform>()->set_position(Math::Vector3f({0, 0, 0}));
        e->get_component<Transform>()->set_scale(
            Math::Vector3f({0.01f, 0.01f, 0.01f}));
        e->get_component<Transform>()->set_euler_angle(
            Math::Vector3f({0, 0, 0}));

        // mesh
        std::vector<uint32_t> indices{};
        for (int i = 0; i < mesh->mNumFaces; i++) {
          indices.push_back(mesh->mFaces[i].mIndices[0]);
          indices.push_back(mesh->mFaces[i].mIndices[1]);
          indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
        e->add_component<Mesh>()->set_indices(indices);
        std::vector<Vertex> vert{};
        for (int i = 0; i < mesh->mNumVertices; i++) {
          Vertex vertex{
              .position =
                  Math::Vector4f({mesh->mVertices[i].x, mesh->mVertices[i].z,
                                  mesh->mVertices[i].y, 1.0f}),
              .normal =
                  Math::Vector3f({mesh->mNormals[i].x, mesh->mNormals[i].y,
                                  mesh->mNormals[i].z}),
              .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
              .texture_coord0 = Math::Vector2f({mesh->mTextureCoords[0][i].x,
                                                -mesh->mTextureCoords[0][i].y}),
          };
          vert.push_back(vertex);
        }
        e->get_component<Mesh>()->set_vertices(vert);

        // material
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        aiString name;
        auto ret =
            material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);
        // std::cout << "  texture=" << name.C_Str() << std::endl;
        auto pos = std::string(name.C_Str()).find_last_of("\\");
        auto len = 1;
        auto path = std::filesystem::directory_entry(
                        "D:/resources/models/Bistro_v5_2/Bistro_v5_2/" +
                        std::string(name.C_Str()).replace(pos, len, "/"))
                        .path();

        // auto r = Engine::get_module<ReflectionRenderer>()->load_texture(
        //     name.C_Str(), path.string());

        // e->add_component<Material>()->id = std::get<0>(r);
        // e->get_component<Material>()->is_transparent = std::get<1>(r);
        e->add_component<Material>()->texture_path = path;
      }
    }
  }
}

std::chrono::system_clock::time_point last_time;
wgpu::Buffer map_buffer;
void main_loop() {
  // frame time
  auto now = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
  last_time = now;
  // std::cout << "frame time: " << duration.count() << "ms" << std::endl;

  // compute test
  if (map_buffer.GetMapState() == wgpu::BufferMapState::Mapped) {
    auto ptr = map_buffer.GetMappedRange();
    auto data = reinterpret_cast<float*>(ptr);
    for (int i = 0; i < 64; ++i) {
      std::cout << data[i] << " ";
    }
    std::cout << std::endl;
    map_buffer.Unmap();
  }
  else if (map_buffer.GetMapState() == wgpu::BufferMapState::Pending) {
    std::cout << "pending...\r" << std::endl;
  }
  else if(map_buffer.GetMapState() == wgpu::BufferMapState::Unmapped) {
    std::cout << "unmapped...\r" << std::endl;
  }

  // user script
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::Z) ==
      ButtonState::DOWN) {
    Engine::get_module<ReflectionRenderer>()->draw_aabb =
        !Engine::get_module<ReflectionRenderer>()->draw_aabb;
  }

  auto renderer = Engine::get_module<ReflectionRenderer>();
  const auto right = Math::Vector3f({std::cos(-renderer->camera_angle.y), 0.0f,
                                     std::sin(-renderer->camera_angle.y)});
  const auto forward =
      -Math::Vector3f({std::sin(-renderer->camera_angle.y), 0.0f,
                       -std::cos(-renderer->camera_angle.y)});
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::A) ==
      ButtonState::HOLD) {
    renderer->camera_position -= 0.1f * right;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::D) ==
      ButtonState::HOLD) {
    renderer->camera_position += 0.1f * right;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
      ButtonState::HOLD) {
    renderer->camera_position += 0.1f * forward;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::S) ==
      ButtonState::HOLD) {
    renderer->camera_position -= 0.1f * forward;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::Q) ==
      ButtonState::HOLD) {
    renderer->camera_position.y -= 0.1f;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::E) ==
      ButtonState::HOLD) {
    renderer->camera_position.y += 0.1f;
  }

  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::H) ==
      ButtonState::HOLD) {
    renderer->camera_angle.y -= 0.1f;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::L) ==
      ButtonState::HOLD) {
    renderer->camera_angle.y += 0.1f;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::J) ==
      ButtonState::HOLD) {
    renderer->camera_angle.x += 0.1f;
  }
  if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::K) ==
      ButtonState::HOLD) {
    renderer->camera_angle.x -= 0.1f;
  }

  Engine::get_module<Input>()->update();
  Engine::get_module<ReflectionRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 1080, 1080);
    Engine::add_module(std::make_shared<Window>(window));
  }

  wgpu::SupportedLimits supported_limits;
  Engine::get_module<Graphics>()->get_device().GetLimits(&supported_limits);
  Engine::get_module<Graphics>()->set_limits(supported_limits.limits);

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->get_instance(),
      Engine::get_module<Graphics>()->get_device());
  Engine::get_module<ReflectionRenderer>()->initialize();

  import();

  // compute shader test
  {
    // on init
    // init compute shader
    wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
    wgsl_desc.code = R"(
      @group(0) @binding(0) var<storage, read> input_buffer: array<f32, 64>;
      @group(0) @binding(1) var<storage, read_write> output_buffer: array<f32, 64>;

      @compute @workgroup_size(32)
      fn computeStuff(@buildin(global_invocation_id) id : vec3<u32>) {
        output_buffer[id.x] = input_buffer[id.x] * 2.0;
      }
    )";
    wgpu::ShaderModuleDescriptor shader_module_desc{
        .nextInChain = &wgsl_desc,
        .label = nullptr,
    };
    wgpu::ShaderModule shader_module =
        Engine::get_module<Graphics>()->get_device().CreateShaderModule(
            &shader_module_desc);
    // create compute pipeline
    wgpu::ComputePipelineDescriptor compute_pipeline_desc{
        .nextInChain = nullptr,
        .compute =
            wgpu::ProgrammableStageDescriptor{
                .module = shader_module,
                .entryPoint = "computeStuff",
            },
    };
    wgpu::ComputePipeline compute_pipeline =
        Engine::get_module<Graphics>()->get_device().CreateComputePipeline(
            &compute_pipeline_desc);
    // initi bind group layout
    std::vector<wgpu::BindGroupLayoutEntry> bindings{
        wgpu::BindGroupLayoutEntry{
            .binding = 0,
            .visibility = wgpu::ShaderStage::Compute,
            .buffer =
                wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::ReadOnlyStorage,
                    .hasDynamicOffset = false,
                },
        },
        wgpu::BindGroupLayoutEntry{
            .binding = 1,
            .visibility = wgpu::ShaderStage::Compute,
            .buffer =
                wgpu::BufferBindingLayout{
                    .type = wgpu::BufferBindingType::Storage,
                },
        },
    };

    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc{
        .nextInChain = nullptr,
        .entryCount = static_cast<uint32_t>(bindings.size()),
        .entries = bindings.data(),
    };
    wgpu::BindGroupLayout bind_group_layout =
        Engine::get_module<Graphics>()->get_device().CreateBindGroupLayout(
            &bind_group_layout_desc);
    // init compute pipeline
    // create compute pipeline layout
    wgpu::PipelineLayoutDescriptor pipeline_layout_desc{
        .bindGroupLayoutCount = 1,
        .bindGroupLayouts = &bind_group_layout,
    };
    wgpu::PipelineLayout pipeline_layout =
        Engine::get_module<Graphics>()->get_device().CreatePipelineLayout(
            &pipeline_layout_desc);
    // init buffers
    const uint64_t buffer_size = 64 * sizeof(float);
    // create input buffer
    wgpu::BufferDescriptor input_buffer_desc{
        .usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst,
        .size = buffer_size,
        .mappedAtCreation = false,
    };
    wgpu::Buffer input_buffer =
        Engine::get_module<Graphics>()->get_device().CreateBuffer(
            &input_buffer_desc);
    // create output buffer
    wgpu::BufferDescriptor output_buffer_desc{
        .usage = wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopySrc,
        .size = buffer_size,
        .mappedAtCreation = false,
    };
    wgpu::Buffer output_buffer =
        Engine::get_module<Graphics>()->get_device().CreateBuffer(
            &output_buffer_desc);

    // create map buffer
    wgpu::BufferDescriptor map_buffer_desc{
        .usage = wgpu::BufferUsage::MapRead | wgpu::BufferUsage::CopyDst,
        .size = buffer_size,
        .mappedAtCreation = false,
    };
    map_buffer =
        Engine::get_module<Graphics>()->get_device().CreateBuffer(
            &map_buffer_desc);

    // init bindgroup
    std::vector<wgpu::BindGroupEntry> bind_group_entries{
        wgpu::BindGroupEntry{
            .binding = 0,
            .buffer = input_buffer,
            .offset = 0,
            .size = buffer_size,
        },
        wgpu::BindGroupEntry{
            .binding = 1,
            .buffer = output_buffer,
            .offset = 0,
            .size = buffer_size,
        },
    };

    wgpu::BindGroupDescriptor bind_group_desc{
        .layout = bind_group_layout,
        .entryCount = static_cast<uint32_t>(bind_group_entries.size()),
        .entries = bind_group_entries.data(),
    };
    wgpu::BindGroup bind_group =
        Engine::get_module<Graphics>()->get_device().CreateBindGroup(
            &bind_group_desc);
    // on compute
    // initialize a command encoder
    wgpu::Queue queue = Engine::get_module<Graphics>()->get_device().GetQueue();
    wgpu::CommandEncoderDescriptor command_encoder_descriptor{
        .nextInChain = nullptr,
        .label = nullptr,
    };
    wgpu::CommandEncoder command_encoder =
        Engine::get_module<Graphics>()->get_device().CreateCommandEncoder(
            &command_encoder_descriptor);

    // create and use compute pass here!
    std::vector<float> input(buffer_size / sizeof(float));
    for (int i = 0; i < input.size(); ++i) {
      input[i] = i;
    }
    queue.WriteBuffer(input_buffer, 0, input.data(), buffer_size);
    // create compute pass
    wgpu::ComputePassDescriptor compute_pass_desc{
        .timestampWriteCount = 0,
        .timestampWrites = nullptr,
    };
    wgpu::ComputePassEncoder compute_pass_encoder =
        command_encoder.BeginComputePass(&compute_pass_desc);

    // use compute pass
    compute_pass_encoder.SetPipeline(compute_pipeline);
    compute_pass_encoder.SetBindGroup(0, bind_group, 0, nullptr);

    uint32_t invocation_count = buffer_size / sizeof(float);
    uint32_t workgroup_size = 32;
    uint32_t wrokgroup_count = (invocation_count + workgroup_size - 1) /
                               workgroup_size;
    compute_pass_encoder.DispatchWorkgroups(wrokgroup_count, 1, 1);

    // finalize compute pass
    compute_pass_encoder.End();
    command_encoder.CopyBufferToBuffer(output_buffer, 0, map_buffer, 0,
                                       buffer_size);

    // encode and submit the gpu commands
    wgpu::CommandBufferDescriptor command_buffer_desc{
        .nextInChain = nullptr,
        .label = nullptr,
    };
    wgpu::CommandBuffer command_buffer =
        command_encoder.Finish(&command_buffer_desc);
    queue.Submit(1, &command_buffer);

    // on finish
    
  }

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

int main() {
  // initialize modules
  Engine::add_module<Graphics>();
  Engine::add_module<Input>();
  // Engine::add_module<ScreenSpaceMeshRenderer>();
  Engine::add_module<ReflectionRenderer>();

  Engine::add_module<EntityStore>();
  Engine::add_module<ScriptStore>();
  Engine::add_module<ScreenSpaceMeshStore>();
  Engine::add_module<ScreenSpaceMaterialStore>();
  Engine::add_module<ScreenSpaceTransformStore>();
  Engine::add_module<MeshStore>();
  Engine::add_module<TransformStore>();
  Engine::add_module<MaterialStore>();

  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });

  Engine::get_module<EntityStore>()->initialize();
  Engine::get_module<ScriptStore>()->initialize();
  Engine::get_module<ScreenSpaceMeshStore>()->initialize();
  Engine::get_module<MeshStore>()->initialize();
  Engine::get_module<TransformStore>()->initialize();
  Engine::get_module<MaterialStore>()->initialize();
  Engine::get_module<ScreenSpaceMaterialStore>()->initialize();
  Engine::get_module<ScreenSpaceTransformStore>()->initialize();
}