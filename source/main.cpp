#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <windows.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <filesystem>
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
        // std::cout << "  mesh=" << mesh->mName.C_Str() << std::endl;
        // std::cout << "  num vertices=" << mesh->mNumVertices << std::endl;
        // std::cout << "  num faces=" << mesh->mNumFaces << std::endl;

        auto e = Engine::get_module<EntityStore>()->create();

        // transform
        e->add_component<Transform>()->set_position(Math::Vector3f({0, 0, 10}));
        e->get_component<Transform>()->set_scale(
            Math::Vector3f({0.001f, 0.001f, 0.001f}));
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
              .texture_coord0 = Math::Vector2f(
                  {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y}),
          };
          vert.push_back(vertex);
        }
        e->get_component<Mesh>()->set_vertices(vert);

        // material
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        aiString name;
        auto ret =
            material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), name);

        std::cout << "  texture=" << name.C_Str() << std::endl;
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
  // std::cout << "frame time: " << duration.count() << "ms" << std::endl;

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
  // auto path =
  //     std::filesystem::directory_entry(
  //         "E:/resources/models/Bistro_v5_2/Bistro_v5_2/Textures/ElectricBox_"
  //         "BaseColor.dds")
  //         .path();
  // DDSLoader::load(path);

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

  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });

  Engine::get_module<EntityStore>()->initialize();
  Engine::get_module<ScriptStore>()->initialize();
  Engine::get_module<ScreenSpaceMeshStore>()->initialize();
  Engine::get_module<MeshStore>()->initialize();
  Engine::get_module<TransformStore>()->initialize();
  Engine::get_module<ScreenSpaceMaterialStore>()->initialize();
  Engine::get_module<ScreenSpaceTransformStore>()->initialize();
}