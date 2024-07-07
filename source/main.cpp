#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <windows.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>
#include <vector>

#include "engine.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "reflection_renderer.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_mesh_renderer.hpp"
#include "script.hpp"
#include "test_image.h"
#include "transform.hpp"
#include "vector.hpp"
#include "window.hpp"
using namespace SF;

std::string transform_to_string(aiMatrix4x4 transform) {
  std::string result = "";
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result += std::to_string(transform[i][j]) + " ";
    }
    result += "\n";
  }
  return result;
}

void search_node(const aiScene* scene, aiNode* node, aiMatrix4x4 transform,
                 int depth = 0) {
  for (int i = 0; i < depth; i++) {
    std::cout << "  ";
  }
  std::cout << "name=" << node->mName.C_Str()
            << " mesh_count=" << node->mNumMeshes << std::endl;

  const aiMatrix4x4 local_transform = node->mTransformation;
  const aiMatrix4x4 global_transform = transform * local_transform;

  if (node->mNumMeshes > 0) {
    const auto mesh = scene->mMeshes[node->mMeshes[0]];
    std::cout << mesh->mFaces[0].mNumIndices << std::endl;
  }

  if (node->mNumChildren > 0) {
    for (int i = 0; i < node->mNumChildren; i++) {
      search_node(scene, node->mChildren[i], global_transform, depth + 1);
    }
  }
  return;
}

void import() {
  const auto filePath =
      "E:/resources/models/Bistro_v5_2/Bistro_v5_2/BistroExterior.fbx";
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
        // in bistro scene, this is camera and directional light
        continue;
      }

      for (int i = 0; i < node->mNumMeshes; i++) {
        auto mesh_index = node->mMeshes[i];
        auto mesh = scene->mMeshes[mesh_index];
        std::cout << "  mesh=" << mesh->mName.C_Str() << std::endl;
        std::cout << "  num vertices=" << mesh->mNumVertices << std::endl;
        std::cout << "  num faces=" << mesh->mNumFaces << std::endl;

        auto e = Engine::get_module<EntityStore>()->create();
        e->add_component<Transform>()->set_position(Math::Vector3f({0, 0, 10}));
        e->get_component<Transform>()->set_scale(
            Math::Vector3f({0.001f, 0.001f, 0.001f}));
        e->get_component<Transform>()->set_euler_angle(
            Math::Vector3f({0, 0, 0}));
        std::vector<uint32_t> indices{};
        for (int i = 0; i < mesh->mNumFaces; i++) {
          indices.push_back(mesh->mFaces[i].mIndices[0]);
          indices.push_back(mesh->mFaces[i].mIndices[1]);
          indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
        e->add_component<Mesh>()->set_indices(indices);
        std::vector<Vertex> vert{};
        for (int i = 0; i < mesh->mNumVertices; i++) {
          // std::cout << "    vertex={" << mesh->mVertices[i].x << " ,"
          //           << mesh->mVertices[i].y << " ," << mesh->mVertices[i].z
          //           << "}" << std::endl;
          Vertex vertex{
              .position =
                  Math::Vector4f({mesh->mVertices[i].x, mesh->mVertices[i].z,
                                  mesh->mVertices[i].y, 1.0f}),
              .normal =
                  Math::Vector3f({mesh->mNormals[i].x, mesh->mNormals[i].y,
                                  mesh->mNormals[i].z}),
              .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
              .texture_coord0 =
                  Math::Vector2f({mesh->mTextureCoords[0][i].x * 0.5f + 0.5f,
                                  mesh->mTextureCoords[0][i].y * 0.5f + 0.5f}),
          };
          vert.push_back(vertex);
        }
        e->get_component<Mesh>()->set_vertices(vert);
      }
    }
  }
}

std::chrono::system_clock::time_point last_time;
void main_loop() {
  // frame time
  auto now = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
  last_time = now;
  std::cout << "frame time: " << duration.count() << "ms" << std::endl;

  // user script

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
  // e = Engine::get_module<EntityStore>()->create();
  // e->add_component<Transform>()->set_position(
  //     Math::Vector3f({0.0f, 0.0f, 1.3f}));
  // e->get_component<Transform>()->set_scale(Math::Vector3f({1.f, 1.f, 1.f}));
  // e->get_component<Transform>()->set_euler_angle(Math::Vector3f({0.1f, 0,
  // 0})); e->add_component<Mesh>()->set_indices(
  //     {0, 1, 2, 0, 2, 3, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4});
  // e->get_component<Mesh>()->set_vertices(
  //     {Vertex{
  //          .position = Math::Vector4f({-0.5f, -0.5f, -0.3f, 1.0f}),
  //          .normal = Math::Vector3f({0.0f, 0.0f, 1.0f}),
  //          .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
  //          .texture_coord0 = Math::Vector2f({0.0f, 0.0f}),
  //      },
  //      Vertex{
  //          .position = Math::Vector4f({0.5f, -0.5f, -0.3f, 1.0f}),
  //          .normal = Math::Vector3f({0.0f, 0.0f, 1.0f}),
  //          .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
  //          .texture_coord0 = Math::Vector2f({1.0f, 0.0f}),
  //      },
  //      Vertex{
  //          .position = Math::Vector4f({0.5f, 0.5f, -0.3f, 1.0f}),
  //          .normal = Math::Vector3f({0.0f, 0.0f, 1.0f}),
  //          .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
  //          .texture_coord0 = Math::Vector2f({1.0f, 1.0f}),
  //      },
  //      Vertex{
  //          .position = Math::Vector4f({-0.5f, 0.5f, -0.3f, 1.0f}),
  //          .normal = Math::Vector3f({0.0f, 0.0f, 1.0f}),
  //          .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
  //          .texture_coord0 = Math::Vector2f({0.0f, 1.0f}),
  //      },
  //      Vertex{
  //          .position = Math::Vector4f({0.0f, 0.0f, 0.5f, 1.0f}),
  //          .normal = Math::Vector3f({0.0f, 0.0f, 1.0f}),
  //          .tangent = Math::Vector4f({0.0f, 0.0f, 0.0f, 0.0f}),
  //          .texture_coord0 = Math::Vector2f({0.5f, 0.5f}),
  //      }});

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
  Engine::add_module<MaterialStore>();
  Engine::add_module<ScreenSpaceTransformStore>();
  Engine::add_module<MeshStore>();
  Engine::add_module<TransformStore>();

  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });

  Engine::get_module<EntityStore>()->initialize();
  Engine::get_module<ScriptStore>()->initialize();
  Engine::get_module<ScreenSpaceMeshStore>()->initialize();
  Engine::get_module<MeshStore>()->initialize();
  Engine::get_module<TransformStore>()->initialize();
  Engine::get_module<MaterialStore>()->initialize();
  Engine::get_module<ScreenSpaceTransformStore>()->initialize();
}