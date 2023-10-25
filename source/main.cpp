#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <tuple>

#include "engine.hpp"
#include "entity.hpp"
#include "screenspace_mesh.hpp"
#include "script.hpp"
#include "test_image.h"

using namespace ShiftFlamework;

// flappy bird
std::shared_ptr<Entity> player;
std::vector<std::shared_ptr<Entity>> pipes{};
float player_a = 0.07;

std::shared_ptr<Entity> background;

bool in_game = false;
float pipe_interval = 1.2f;
float pipe_randomness_coeff = 0.1f;
float pipe_gap = 0.4f;
float pipe_speed = 0.5f / 60.0f;
float flame_sinse_last_pipe = 0;
float player_rad = 0.2f;
float pipe_width = 0.3f;
std::queue<std::shared_ptr<Entity>> unused_pipe{};

void restart_flappy() {
  while (!unused_pipe.empty()) unused_pipe.pop();
  for (int i = 0; i < pipes.size(); i++) {
    auto pipe_t = pipes.at(i)->get_component<ScreenSpaceTransform>();
    pipe_t->position = Math::Vector2f({-1.5f, i % 2 == 0.0f ? -1.0f : 1.0f});
    pipe_t->scale = Math::Vector2f({pipe_width, 1.0f});
  }

  auto p_transform = player->get_component<ScreenSpaceTransform>();
  p_transform->scale = Math::Vector2f({0.2f, 0.2f});
  p_transform->position = Math::Vector2f({-0.3, 0});

  flame_sinse_last_pipe = 0;
  player_a = 0.07;
}

void main_loop() {
  // user script
  if (in_game) {
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::ESCAPE) ==
        ButtonState::DOWN) {
      in_game = false;
      restart_flappy();
    }

    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::W) ==
        ButtonState::DOWN) {
      player_a = 0.07f;
    } else {
      player_a -= 0.007f;
    }
    player->get_component<ScreenSpaceTransform>()->position =
        player->get_component<ScreenSpaceTransform>()->position +
        Math::Vector2f({0, player_a});
    float current_y = player->get_component<ScreenSpaceTransform>()
                          ->position.internal_data[1];
    if (current_y < -1.2 || current_y > 1.0) {
      in_game = false;
      restart_flappy();
    }

    player->get_component<ScreenSpaceTransform>()->angle =
        (player_a <= -0.03) ? -0.4f : 0.0f;

    // respawn pipe
    flame_sinse_last_pipe++;
    if (flame_sinse_last_pipe > pipe_interval / pipe_speed) {
      flame_sinse_last_pipe = 0;
      std::random_device seed_gen;
      std::mt19937 engine(seed_gen());
      std::normal_distribution<float> r(-pipe_randomness_coeff,
                                        pipe_randomness_coeff);
      auto rand = r(engine);
      for (int i = 0; i < 2; i++) {
        unused_pipe.front()
            ->get_component<ScreenSpaceTransform>()
            ->position.internal_data[0] = 1.5f;
        unused_pipe.front()
            ->get_component<ScreenSpaceTransform>()
            ->position.internal_data[1] = (i % 2 == 0.0f ? -1.0f : 1.0f) + rand;
        unused_pipe.pop();
      }
    }
    for (int i = 0; i < pipes.size(); i++) {
      auto pipe_t = pipes.at(i)->get_component<ScreenSpaceTransform>();
      pipe_t->position.internal_data[0] -= pipe_speed;

      if (pipe_t->position.internal_data[0] < -1.5f) {
        unused_pipe.push(pipes.at(i));
      }
    }

    // hit
    for (int i = 0; i < pipes.size(); i++) {
      auto pipe = pipes.at(i);
      auto pipe_t = pipe->get_component<ScreenSpaceTransform>();
      if (std::abs(pipe_t->position.internal_data[0] -
                   player->get_component<ScreenSpaceTransform>()
                       ->position.internal_data[0]) <
          (pipe_width + player_rad * 0.8f) / 2.0f) {
        if (std::abs(pipe_t->position.internal_data[1] -
                     player->get_component<ScreenSpaceTransform>()
                         ->position.internal_data[1]) <
            (1.0f + player_rad * 0.8f) / 2.0f) {
          in_game = false;
          restart_flappy();
        }
      }
    }
  } else {
    // not in game
    if (Engine::get_module<Input>()->get_keyboard_state(Keyboard::R) ==
        ButtonState::DOWN) {
      in_game = true;
    }
  }

  Engine::get_module<Input>()->update();
  Engine::get_module<ScreenSpaceMeshRenderer>()->render(
      Engine::get_module<Window>()->get_swap_chain().GetCurrentTextureView());
}

void start() {
  {
    Window window("game window", 1080, 1080);
    std::get<std::shared_ptr<Window>>(Engine::modules) =
        std::make_shared<Window>(window);
  }

  wgpu::SupportedLimits supported_limits;
  Engine::get_module<Graphics>()->device.GetLimits(&supported_limits);
  Engine::get_module<Graphics>()->limits = supported_limits.limits;

  Engine::get_module<Window>()->initialize_swap_chain(
      Engine::get_module<Graphics>()->instance,
      Engine::get_module<Graphics>()->device);
  Engine::get_module<ScreenSpaceMeshRenderer>()->initialize();

  // game initialize
  // background
  background = std::make_shared<Entity>();
  background->add_component<ScreenSpaceMesh>();
  background->add_component<ScreenSpaceTransform>()->scale =
      Math::Vector2f({2, 2});

  // pipe
  pipes = std::vector<std::shared_ptr<Entity>>(8, nullptr);
  for (int i = 0; i < pipes.size(); i++) {
    auto pipe = std::make_shared<Entity>();
    pipe->add_component<ScreenSpaceMesh>();
    auto pipe_t = pipe->add_component<ScreenSpaceTransform>();
    pipe_t->position = Math::Vector2f({-1.5f, i % 2 == 0.0f ? -1.0f : 1.0f});
    pipe_t->scale = Math::Vector2f({pipe_width, 1.0f});
    pipes.at(i) = pipe;
    unused_pipe.push(pipe);
  }

  // player
  player = std::make_shared<Entity>();
  player->add_component<ScreenSpaceMesh>();
  player->add_component<ScreenSpaceTransform>();
  auto p_transform = player->get_component<ScreenSpaceTransform>();
  p_transform->scale = Math::Vector2f({player_rad, player_rad});
  p_transform->position = Math::Vector2f({-0.3, 0});

  // start main loop
  Engine::get_module<Window>()->start_main_loop(main_loop);
}

// pointer to modules
std::tuple<std::shared_ptr<Graphics>, std::shared_ptr<Window>,
           std::shared_ptr<Input>, std::shared_ptr<ScreenSpaceMeshRenderer>>
    Engine::modules = std::make_tuple(nullptr, nullptr, nullptr, nullptr);

int main() {
  // initialize modules
  std::get<std::shared_ptr<Graphics>>(Engine::modules) =
      std::make_shared<Graphics>();
  std::get<std::shared_ptr<Input>>(Engine::modules) = std::make_shared<Input>();
  std::get<std::shared_ptr<ScreenSpaceMeshRenderer>>(Engine::modules) =
      std::make_shared<ScreenSpaceMeshRenderer>();
  Engine::get_module<Input>()->initialize();
  Engine::get_module<Graphics>()->initialize([]() { start(); });
}