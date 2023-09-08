#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>

#include <iostream>
#include <memory>
#include <tuple>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

#include "engine.hpp"

using namespace ShiftFlamework;

wgpu::RenderPipeline pipeline;

const uint32_t k_width = 512;
const uint32_t k_height = 512;

const char shader_code[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

void create_render_pipeline() {
  wgpu::ShaderModuleWGSLDescriptor wgsl_desc{};
  wgsl_desc.code = shader_code;

  wgpu::ShaderModuleDescriptor shader_module_desc{.nextInChain = &wgsl_desc};
  wgpu::ShaderModule shader_module =
      Engine::GetModule<Graphics>()->device.CreateShaderModule(
          &shader_module_desc);

  wgpu::ColorTargetState color_target_state{
      .format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragment_state{.module = shader_module,
                                     .entryPoint = "fragmentMain",
                                     .targetCount = 1,
                                     .targets = &color_target_state};

  wgpu::RenderPipelineDescriptor render_pipeline_desc{
      .vertex = {.module = shader_module, .entryPoint = "vertexMain"},
      .fragment = &fragment_state};
  pipeline = Engine::GetModule<Graphics>()->device.CreateRenderPipeline(
      &render_pipeline_desc);
}

void render() {
  wgpu::RenderPassColorAttachment attachment{
      .view =
          Engine::GetModule<Window>()->get_swap_chain().GetCurrentTextureView(),
      .loadOp = wgpu::LoadOp::Clear,
      .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                        .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder =
      Engine::GetModule<Graphics>()->device.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(pipeline);
  pass.Draw(3);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  Engine::GetModule<Graphics>()->device.GetQueue().Submit(1, &commands);
}

void main_loop() { render(); }

void start() {
  {
    Window window("game window", k_width, k_height);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  Engine::GetModule<Window>()->initialize_swap_chain(
      Engine::GetModule<Graphics>()->instance,
      Engine::GetModule<Graphics>()->device);
  create_render_pipeline();

  Engine::GetModule<Window>()->start_main_loop(main_loop);
}

std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>> Engine::modules;

int main() {
  std::cout << "game start!" << std::endl;
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  Engine::GetModule<Graphics>()->instance = wgpu::CreateInstance();
  Engine::GetModule<Graphics>()->initialize([]() { start(); });
}