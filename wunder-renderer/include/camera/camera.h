/*
 * Copyright (c) 2018-2024, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024, NVIDIA CORPORATION.
 * SPDX-License-Identifier: Apache-2.0
 */
//--------------------------------------------------------------------

#ifndef WUNDER_CAMERA_H
#define WUNDER_CAMERA_H

#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>

#include "core/input.h"
#include "core/time_unit.h"
#include "core/wunder_memory.h"
#include "event/event_handler.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "resources/shaders/host_device.h"

namespace wunder {
namespace vulkan {

class renderer;
}
namespace event {
struct scene_activated;
namespace mouse {
struct move;
struct scroll;
}  // namespace mouse
}  // namespace event
}  // namespace wunder

namespace wunder {

class camera : private event_handler<wunder::event::mouse::move>,
               private event_handler<wunder::event::mouse::scroll>,
               private event_handler<wunder::event::scene_activated>

{
 public:
  // clang-format off
    enum class modes { examine, fly, walk};
    enum class actions { no_action, orbit, dolly, pan, look_around };
  // clang-format on

  struct properties {
    glm::vec3 eye = glm::vec3(10, 10, 10);
    glm::vec3 ctr = glm::vec3(0, 0, 0);
    glm::vec3 up = glm::vec3(0, 1, 0);
    float fov = 60.0f;

    bool operator!=(const properties& rhr) const {
      return (eye != rhr.eye) || (ctr != rhr.ctr) || (up != rhr.up) ||
             (fov != rhr.fov);
    }
    bool operator==(const properties& rhr) const {
      return (eye == rhr.eye) && (ctr == rhr.ctr) && (up == rhr.up) &&
             (fov == rhr.fov);
    }
  };

 public:
  camera();
  ~camera();

 public:
  void update(time_unit dt);
  void update_movement(time_unit dt);

 public:
  void set_fov(float _fov);

  void set_camera_properties(properties camera, bool instantSet = true);

  // Set the camera to look at the interest point
  // instantSet = true will not interpolate to the new position
  void set_lookat(const glm::vec3& eye, const glm::vec3& center,
                  const glm::vec3& up, bool instantSet = true);
  // To call when the size of the window change.  This allows to do nicer
  // movement according to the window size.
  void set_window_size(int w, int h);
  // Retrieve the position, interest and up vector of the camera
  void get_lookat(glm::vec3& eye, glm::vec3& center, glm::vec3& up) const;

  // Set the position, interest from the matrix.
  // instantSet = true will not interpolate to the new position
  // centerDistance is the distance of the center from the eye
  void set_view_matrix(const glm::mat4& view_matrix, bool instantSet = true,
                       float centerDistance = 1.f);

  // Fitting the camera position and interest to see the bounding box
  void fit(const glm::vec3& boxMin, const glm::vec3& boxMax,
           bool instantFit = true, bool tight = false, float aspect = 1.0f);

  void bind(wunder::vulkan::renderer& renderer);
 public:
  // field of view in degrees
  float get_fov() { return m_current.fov; }

  glm::vec3 get_eye() const { return m_current.eye; }
  glm::vec3 get_center() const { return m_current.ctr; }
  glm::vec3 get_up() const { return m_current.up; }

  // retrieve the screen dimension
  int get_width() const { return m_width; }
  int get_height() const { return m_height; }
  float get_aspect_ratio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
  }

  // set the manipulator mode, from examiner, to walk, to fly, ...
  void set_mode(modes mode) { m_mode = mode; }
  // retrieve the current manipulator mode
  modes get_mode() const { return m_mode; }

  // Retrieving the transformation matrix of the camera
  const glm::mat4& get_view_matrix() const { return m_view_matrix; }

  // Changing the default speed movement
  void set_speed(float speed) { m_speed = speed; }

  // Retrieving the current speed
  float get_speed() const { return m_speed; }

  properties get_camera_properties() const { return m_current; }

  // Clip planes
  void set_clip_planes(glm::vec2 clip) { m_clip_planes = clip; }
  const glm::vec2& get_clip_planes() const { return m_clip_planes; }

  // Animation duration
  double get_animation_duration() const { return m_duration; }
  void set_animation_duration(double val) { m_duration = val; }
  bool is_animated() const { return m_anim_done == false; }

 public:
  void on_event(const wunder::event::mouse::move& event) override;
  void on_event(const wunder::event::mouse::scroll& event) override;
  void on_event(const wunder::event::scene_activated& event) override;

 private:
  // This should be called in an application loop to update the camera matrix if
  // this one is animated: new position, key movement
  void update_camera_position_smoothly();
  // Update the internal matrix.
  void update_view_matrix() {
    m_view_matrix = glm::lookAt(m_current.eye, m_current.ctr, m_current.up);
  }

 private:
  // Main function which is called to apply a camera motion.
  // It is preferable to
  void rotate(glm::vec2, actions action = actions::no_action);

  void move(float dx, float dy, camera::actions action);

 private:
  // Do panning: movement parallels to the screen
  void pan(float dx, float dy);
  // Do orbiting: rotation around the center of interest. If invert, the
  // interest orbit around the camera position
  void orbit(float dx, float dy, bool invert = false);
  // Do dolly: movement toward the interest.
  void dolly(float dx, float dy);

 private:
  glm::vec3 compute_bezier(float t, glm::vec3& p0, glm::vec3& p1,
                           glm::vec3& p2);
  void find_bezier_points();

 protected:
  glm::mat4 m_view_matrix = glm::mat4(1);

  properties m_current;   // Current camera position
  properties m_goal;      // Wish camera position
  properties m_snapshot;  // Current camera the moment a set look-at is done

  // Animation
  std::array<glm::vec3, 3> m_bezier;
  double m_start_time = 0;
  double m_duration = 0.5;
  bool m_anim_done{true};
  glm::vec3 m_camera_position_difference{0, 0, 0};

  // Screen
  int m_width = 1;
  int m_height = 1;

  // Other
  float m_speed = 3.f;
  glm::vec2 m_clip_planes = glm::vec2(0.001f, 100000000.f);

  modes m_mode;
  std::unordered_map<camera::actions, std::function<void(float dx, float dy)>>
      m_action_fns;

  unique_ptr<vulkan::uniform_buffer> m_camera_buffer;
  void update_camera_buffer();
  SceneCamera create_host_camera();
};

}  // namespace wunder

#endif  // WUNDER_CAMERA_H
