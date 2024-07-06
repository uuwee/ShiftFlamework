#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <windows.h>

#include <assimp/Importer.hpp>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "graphics.hpp"
#include "input.hpp"
#include "material.hpp"
#include "screenspace_mesh.hpp"
#include "screenspace_mesh_renderer.hpp"
#include "script.hpp"
#include "test_image.h"
#include "vector.hpp"
#include "window.hpp"
using namespace SF;

std::shared_ptr<Entity> e;

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

void search_node(aiNode* node, int depth = 0) {
  for (int i = 0; i < depth; i++) {
    std::cout << "  ";
  }
  std::cout << "name=" << node->mName.C_Str()
            << " transform=" << transform_to_string(node->mTransformation)
            << std::endl;
  if (node->mNumChildren > 0) {
    for (int i = 0; i < node->mNumChildren; i++) {
      search_node(node->mChildren[i], depth + 1);
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
    search_node(scene->mRootNode);
  }
}

void main_loop() {
  // user script
  e->get_component<Script>()->update();

  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
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
  Engine::get_module<ScreenSpaceMeshRenderer>()->initialize();

  // e = std::make_shared<Entity>();
  e = Engine::get_module<EntityStore>()->create();
  e->add_component<Script>();

  import();

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

int main() {
  // initialize modules
  Engine::add_module<Graphics>();
  Engine::add_module<Input>();
  Engine::add_module<ScreenSpaceMeshRenderer>();

  Engine::add_module<EntityStore>();
  Engine::add_module<ScriptStore>();
  Engine::add_module<ScreenSpaceMeshStore>();
  Engine::add_module<MaterialStore>();
  Engine::add_module<ScreenSpaceTransformStore>();

  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });

  Engine::get_module<EntityStore>()->initialize();
  Engine::get_module<ScriptStore>()->initialize();
  Engine::get_module<ScreenSpaceMeshStore>()->initialize();
  Engine::get_module<MaterialStore>()->initialize();
  Engine::get_module<ScreenSpaceTransformStore>()->initialize();
}