#include "camera/camera.h"

#include <chrono>
#include <iostream>

#include "assets/scene_asset.h"
#include "core/input_manager.h"
#include "core/project.h"
#include "core/services_factory.h"
#include "core/wunder_features.h"
#include "event/camera_events.h"
#include "event/event_handler.hpp"
#include "event/input_events.h"
#include "event/scene_events.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "gla/vulkan/scene/vulkan_scene.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "resources/shaders/host_device.h"
#include "scene/scene_manager.h"

namespace wunder {
namespace {
auto k_pressed_or_held_key = key_state::pressed | key_state::held;
}

camera::camera()
    : m_action_fns(
          {{camera::actions::orbit,
            [this](float dx, float dy) { orbit(dx, dy, false); }},
           {camera::actions::dolly,
            [this](float dx, float dy) { dolly(dx, dy); }},
           {camera::actions::pan, [this](float dy, float dx) { pan(dx, dy); }},
           {camera::actions::look_around,
            [this](float dx, float dy) { orbit(dx, dy, true); }}}) {
  update_view_matrix();
  SceneCamera camera = create_host_camera();

  m_camera_buffer.reset(new vulkan::uniform_device_buffer(
      {.m_enabled = true, .m_descriptor_name = "_SceneCamera"}, &camera,
      sizeof(SceneCamera), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT));
}

camera::~camera() {
  if (m_camera_buffer) {
    m_camera_buffer.reset();
  }
}

void camera::update(time_unit dt) {
  update_camera_position_smoothly();
  update_camera_buffer();
  update_movement(dt);
}

void camera::update_movement(time_unit dt) {
  auto& input_manager = service_factory::instance().get_input_manager();

  bool is_control_pressed =
      input_manager.is_key_in_state(wunder::keyboard::key_code::left_control,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::right_control,
                                    k_pressed_or_held_key);

  bool is_alt_pressed =
      input_manager.is_key_in_state(wunder::keyboard::key_code::left_alt,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::right_alt,
                                    k_pressed_or_held_key);

  bool is_shift_pressed =
      input_manager.is_key_in_state(wunder::keyboard::key_code::left_shift,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::right_shift,
                                    k_pressed_or_held_key);

  ReturnIf(is_control_pressed || is_alt_pressed || is_shift_pressed);

  auto factor = static_cast<float>(dt.m_miliseconds) * 0.1f;
  m_camera_position_difference = {0, 0, 0};

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::w,
                                    k_pressed_or_held_key)) {
    move(factor, 0, actions::dolly);
  }

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::up,
                                    k_pressed_or_held_key)) {
    move(0, factor, actions::pan);
  }

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::s,
                                    k_pressed_or_held_key)) {
    move(-factor, 0, actions::dolly);
  }

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::down,
                                    k_pressed_or_held_key)) {
    move(0, -factor, actions::pan);
  }

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::d,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::right,
                                    k_pressed_or_held_key)) {
    move(factor, 0, actions::pan);
  }

  if (input_manager.is_key_in_state(wunder::keyboard::key_code::a,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::left,
                                    k_pressed_or_held_key)) {
    move(-factor, 0, actions::pan);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// Set and clamp FOV between 0.01 and 179 degrees
/////////////////////////////////////////////////////////////////////////////////////////
void camera::set_fov(float _fov) {
  m_current.fov = std::min(std::max(_fov, 0.01f), 179.0f);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Set the new camera as a goal
/////////////////////////////////////////////////////////////////////////////////////////
void camera::set_camera_properties(properties camera,
                                   bool instantSet /*=true*/) {
  m_anim_done = true;

  if (instantSet || m_duration == 0.0) {
    m_current = camera;
    update_view_matrix();
  } else if (camera != m_current) {
    m_goal = camera;
    m_snapshot = m_current;
    m_anim_done = false;
    m_start_time = get_system_time();
    find_bezier_points();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// Creates a viewing matrix derived from an eye point, a reference point
// indicating the center of the scene, and an up vector
/////////////////////////////////////////////////////////////////////////////////////////
void camera::set_lookat(const glm::vec3& eye, const glm::vec3& center,
                        const glm::vec3& up, bool instantSet) {
  properties camera{eye, center, up, m_current.fov};
  set_camera_properties(camera, instantSet);
}

void camera::set_window_size(int w, int h) {
  m_width = w;
  m_height = h;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Get the current camera's look-at parameters.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::get_lookat(glm::vec3& eye, glm::vec3& center,
                        glm::vec3& up) const {
  eye = m_current.eye;
  center = m_current.ctr;
  up = m_current.up;
}

void camera::set_view_matrix(const glm::mat4& view_matrix, bool instantSet,
                             float centerDistance) {
  properties camera;
  camera.eye = view_matrix[3];  // that's our transform part of the view_matrix

  auto rotMat = glm::mat3(view_matrix);
  camera.ctr = {0, 0, -centerDistance};
  camera.ctr = camera.eye + (rotMat * camera.ctr);
  camera.up = {0, 1, 0};
  camera.fov = m_current.fov;

  m_anim_done = instantSet;

  if (instantSet) {
    m_current = camera;
  } else {
    m_goal = camera;
    m_snapshot = m_current;
    m_start_time = get_system_time();
    find_bezier_points();
  }

  update_view_matrix();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Move the camera closer or further from the center of the the bounding box, to
// see it completely
//
// boxMin - lower corner of the bounding box
// boxMax - upper corner of the bounding box
// instantFit - true: set the new position, false: will animate to new position.
// tight - true: fit exactly the corner, false: fit to radius (larger view, will
// not get closer or further away) aspect - aspect ratio of the window.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::fit(const glm::vec3& boxMin, const glm::vec3& boxMax,
                 bool instantFit /*= true*/, bool tightFit /*=false*/,
                 float aspect /*=1.0f*/) {
  // Calculate the half extents of the bounding box
  const glm::vec3 boxHalfSize = 0.5f * (boxMax - boxMin);

  // Calculate the center of the bounding box
  const glm::vec3 boxCenter = 0.5f * (boxMin + boxMax);

  const float yfov = std::tan(glm::radians(m_current.fov * 0.5f));
  const float xfov = yfov * aspect;

  // Calculate the ideal distance for a tight fit or fit to radius
  float idealDistance = 0;

  if (tightFit) {
    // Get only the rotation matrix
    glm::mat3 mView = glm::lookAt(m_current.eye, boxCenter, m_current.up);

    // Check each 8 corner of the cube
    for (int i = 0; i < 8; i++) {
      // Rotate the bounding box in the camera view
      glm::vec3 vct(i & 1 ? boxHalfSize.x : -boxHalfSize.x,   //
                    i & 2 ? boxHalfSize.y : -boxHalfSize.y,   //
                    i & 4 ? boxHalfSize.z : -boxHalfSize.z);  //
      vct = mView * vct;

      if (vct.z < 0)  // Take only points in front of the center
      {
        // Keep the largest offset to see that vertex
        idealDistance =
            std::max(float(fabs(vct.y) / yfov + fabs(vct.z)), idealDistance);
        idealDistance =
            std::max(float(fabs(vct.x) / xfov + fabs(vct.z)), idealDistance);
      }
    }
  } else  // Using the bounding sphere
  {
    const float radius = glm::length(boxHalfSize);
    idealDistance = std::max(radius / xfov, radius / yfov);
  }

  // Calculate the new camera position based on the ideal distance
  const glm::vec3 newEye =
      boxCenter - idealDistance * glm::normalize(boxCenter - m_current.eye);

  // Set the new camera position and interest point
  set_lookat(newEye, boxCenter, m_current.up, instantFit);
}

void camera::collect_descriptors(vulkan::descriptor_set_manager& target) {
  m_camera_buffer->add_descriptor_to(target);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Modify the position of the camera over time
// - The camera can be updated through keys. A key set a direction which is
// added to both
//   eye and center, until the key is released
// - A new position of the camera is defined and the camera will reach that
// position
//   over time.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::update_camera_position_smoothly() {
  auto elapse = static_cast<float>(get_system_time() - m_start_time) / 1000.f;

  // Key animation
  if (m_camera_position_difference != glm::vec3(0, 0, 0)) {
    m_current.eye += m_camera_position_difference * elapse;
    m_current.ctr += m_camera_position_difference * elapse;
    update_view_matrix();
    m_start_time = get_system_time();
    return;
  }

  // properties moving to new position
  if (m_anim_done) return;

  float t = std::min(elapse / float(m_duration), 1.0f);
  // Evaluate polynomial (smoother step from Perlin)
  t = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
  if (t >= 1.0f) {
    m_current = m_goal;
    m_anim_done = true;
    update_view_matrix();
    return;
  }

  // Interpolate camera position and interest
  // The distance of the camera between the interest is preserved to
  // create a nicer interpolation
  m_current.ctr = glm::mix(m_snapshot.ctr, m_goal.ctr, t);
  m_current.up = glm::mix(m_snapshot.up, m_goal.up, t);
  m_current.eye = compute_bezier(t, m_bezier[0], m_bezier[1], m_bezier[2]);
  m_current.fov = glm::mix(m_snapshot.fov, m_goal.fov, t);

  update_view_matrix();
}
/////////////////////////////////////////////////////////////////////////////////////////
void camera::update_view_matrix() {
  m_view_matrix = glm::lookAt(m_current.eye, m_current.ctr, m_current.up);
  print_camera_angles();

  event_controller::on_event(event::camera_moved{});
}

/////////////////////////////////////////////////////////////////////////////////////////
// Low level function for when the camera move.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::rotate(glm::vec2 new_position, actions action) {
  auto& input_manager = service_factory::instance().get_input_manager();
  const glm::vec2& old_position = input_manager.get_old_mouse_position();

  float dx = float(new_position.x - old_position.x) / float(m_width);
  float dy = float(new_position.y - old_position.y) / float(m_height);

  auto action_fn_it = m_action_fns.find(action);
  AssertReturnIf(action_fn_it == m_action_fns.end());

  action_fn_it->second(dx, dy);

  // Resetting animation
  m_anim_done = true;

  update_view_matrix();
}

//
// Function for when the camera move with keys (ex. WASD).
//
void camera::move(float dx, float dy, camera::actions action) {
  auto d = glm::normalize(m_current.ctr - m_current.eye);
  dx *= m_speed;
  dy *= m_speed;

  glm::vec3 key_vec;
  if (action == actions::dolly) {
    key_vec = d * dx;

    if (m_mode == modes::walk) {
      if (m_current.up.y > m_current.up.z) {
        key_vec.y = 0;
      } else {
        key_vec.z = 0;
      }
    }
  } else if (action == actions::pan) {
    auto r = glm::cross(d, m_current.up);
    key_vec = r * dx + m_current.up * dy;
  }

  m_camera_position_difference += key_vec;

  // Resetting animation
  m_start_time = get_system_time();
}

/////////////////////////////////////////////////////////////////////////////////////////
// To call when the mouse is moving
// It find the appropriate camera operator, based on the mouse button pressed
// and the keyboard modifiers (shift, ctrl, alt)
//
// Returns the action that was activated
/////////////////////////////////////////////////////////////////////////////////////////
void camera::on_event(const wunder::event::mouse::move& event) {
  auto& input_manager = service_factory::instance().get_input_manager();

  bool is_left_mouse_button_pressed = input_manager.is_key_in_state(
      wunder::mouse::key_code::button_left, k_pressed_or_held_key);
  bool is_right_mouse_button_pressed = input_manager.is_key_in_state(
      wunder::mouse::key_code::button_right, k_pressed_or_held_key);
  bool is_middle_mouse_button_pressed = input_manager.is_key_in_state(
      wunder::mouse::key_code::button_middle, k_pressed_or_held_key);

  if (!is_left_mouse_button_pressed && !is_right_mouse_button_pressed &&
      !is_middle_mouse_button_pressed) {
    return;  // no mouse button pressed
  }

  actions curAction = actions::no_action;
  if (is_left_mouse_button_pressed) {
    bool is_control_pressed =
        input_manager.is_key_in_state(wunder::keyboard::key_code::left_control,
                                      k_pressed_or_held_key) ||
        input_manager.is_key_in_state(wunder::keyboard::key_code::right_control,
                                      k_pressed_or_held_key);

    bool is_shift_pressed =
        input_manager.is_key_in_state(wunder::keyboard::key_code::left_shift,
                                      k_pressed_or_held_key) ||
        input_manager.is_key_in_state(wunder::keyboard::key_code::right_shift,
                                      k_pressed_or_held_key);

    bool is_alt_pressed =
        input_manager.is_key_in_state(wunder::keyboard::key_code::left_alt,
                                      k_pressed_or_held_key) ||
        input_manager.is_key_in_state(wunder::keyboard::key_code::right_alt,
                                      k_pressed_or_held_key);

    if ((is_control_pressed && is_shift_pressed) || is_alt_pressed) {
      curAction =
          m_mode == modes::examine ? actions::look_around : actions::orbit;
    } else if (is_shift_pressed) {
      curAction = camera::actions::dolly;
    } else if (is_control_pressed) {
      curAction = camera::actions::pan;
    } else {
      curAction = m_mode == camera::modes::examine
                      ? camera::actions::orbit
                      : camera::actions::look_around;
    }
  } else if (is_middle_mouse_button_pressed) {
    curAction = camera::actions::pan;
  } else {
    curAction = camera::actions::dolly;
  }

  rotate(event.m_position, curAction);
}

/////////////////////////////////////////////////////////////////////////////////////////
// Trigger a dolly when the wheel change, or change the FOV if the shift key was
// pressed
/////////////////////////////////////////////////////////////////////////////////////////
void camera::on_event(const wunder::event::mouse::scroll& event) /*override*/
{
  auto& input_manager = service_factory::instance().get_input_manager();
  bool is_shift_pressed =
      input_manager.is_key_in_state(wunder::keyboard::key_code::left_shift,
                                    k_pressed_or_held_key) ||
      input_manager.is_key_in_state(wunder::keyboard::key_code::right_shift,
                                    k_pressed_or_held_key);

  auto fval(static_cast<float>(event.m_offset.y));
  float dx = (fval * fabsf(fval)) / static_cast<float>(m_width);

  if (is_shift_pressed) {
    set_fov(m_current.fov + fval);
  } else {
    dolly(dx * m_speed, dx * m_speed);
    update_view_matrix();
  }
}

void camera::on_event(
    const wunder::event::scene_activated& event) /*override*/ {
  auto scene_asset =
      project::instance().get_scene_manager().get_scene_asset(event.m_id);
  AssertReturnUnless(scene_asset);

  auto api_scene =
      project::instance().get_scene_manager().mutable_api_scene(event.m_id);
  AssertReturnUnless(api_scene);

  m_lights_count = 0;

  const aabb& scene_aabb = scene_asset->get().get_aabb();
  fit(scene_aabb.m_min, scene_aabb.m_max);
}

// Bezier helper functions
// TODO:: Move if they're needed from some other place

glm::vec3 camera::compute_bezier(float t, glm::vec3& p0, glm::vec3& p1,
                                 glm::vec3& p2) {
  float u = 1.f - t;
  float tt = t * t;
  float uu = u * u;

  glm::vec3 p = uu * p0;  // first term
  p += 2 * u * t * p1;    // second term
  p += tt * p2;           // third term

  return p;
}

void camera::find_bezier_points() {
  glm::vec3 p0 = m_current.eye;
  glm::vec3 p2 = m_goal.eye;
  glm::vec3 p1, pc;

  // point of interest
  glm::vec3 pi = (m_goal.ctr + m_current.ctr) * 0.5f;

  glm::vec3 p02 = (p0 + p2) * 0.5f;                           // mid p0-p2
  float radius = (length(p0 - pi) + length(p2 - pi)) * 0.5f;  // Radius for p1
  glm::vec3 p02pi(p02 - pi);  // Vector from interest to mid point
  p02pi = glm::normalize(p02pi);
  p02pi *= radius;
  pc = pi + p02pi;                        // Calculated point to go through
  p1 = 2.f * pc - p0 * 0.5f - p2 * 0.5f;  // Computing p1 for t=0.5
  p1.y = p02.y;  // Clamping the P1 to be in the same height as p0-p2

  m_bezier[0] = p0;
  m_bezier[1] = p1;
  m_bezier[2] = p2;
}

// Camera rotation starts here

/////////////////////////////////////////////////////////////////////////////////////////
// Pan the camera perpendicularly to the light of sight.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::pan(float dx, float dy) {
  if (m_mode == modes::fly) {
    dx *= -1;
    dy *= -1;
  }

  glm::vec3 z(m_current.eye - m_current.ctr);
  float length = static_cast<float>(glm::length(z)) / 0.785f;  // 45 degrees
  z = glm::normalize(z);
  glm::vec3 x = glm::cross(m_current.up, z);
  glm::vec3 y = glm::cross(z, x);
  x = glm::normalize(x);
  y = glm::normalize(y);

  glm::vec3 panVector = (-dx * x + dy * y) * length;
  m_current.eye += panVector;
  m_current.ctr += panVector;
}

/////////////////////////////////////////////////////////////////////////////////////////
// orbit the camera around the center of interest. If 'invert' is true,
// then the camera stays in place and the interest orbit around the camera.
/////////////////////////////////////////////////////////////////////////////////////////
void camera::orbit(float dx, float dy, bool invert) {
  if (dx == 0 && dy == 0) return;

  // Full width will do a full turn
  dx *= glm::two_pi<float>();
  dy *= glm::two_pi<float>();

  // Get the camera
  glm::vec3 origin(invert ? m_current.eye : m_current.ctr);
  glm::vec3 position(invert ? m_current.ctr : m_current.eye);

  // Get the length of sight
  glm::vec3 centerToEye(position - origin);
  float radius = glm::length(centerToEye);
  centerToEye = glm::normalize(centerToEye);
  glm::vec3 axe_z = centerToEye;

  // Find the rotation around the UP axis (Y)
  glm::mat4 rot_y = glm::rotate(glm::mat4(1), -dx, m_current.up);

  // Apply the (Y) rotation to the eye-center vector
  centerToEye = rot_y * glm::vec4(centerToEye, 0);

  // Find the rotation around the X vector: cross between eye-center and up (X)
  glm::vec3 axe_x = glm::normalize(glm::cross(m_current.up, axe_z));
  glm::mat4 rot_x = glm::rotate(glm::mat4(1), -dy, axe_x);

  // Apply the (X) rotation to the eye-center vector
  glm::vec3 vect_rot = rot_x * glm::vec4(centerToEye, 0);

  if (glm::sign(vect_rot.x) == glm::sign(centerToEye.x)) centerToEye = vect_rot;

  // Make the vector as long as it was originally
  centerToEye *= radius;

  // Finding the new position
  glm::vec3 newPosition = centerToEye + origin;

  if (!invert) {
    m_current.eye = newPosition;  // Normal: change the position of the camera
  } else {
    m_current.ctr = newPosition;  // Inverted: change the interest point
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
// Move the camera toward the interest point, but don't cross it
/////////////////////////////////////////////////////////////////////////////////////////
void camera::dolly(float dx, float dy) {
  glm::vec3 z = m_current.ctr - m_current.eye;
  auto length = static_cast<float>(glm::length(z));

  // We are at the point of interest, and don't know any direction, so do
  // nothing!
  if (length < 0.000001f) return;

  // Use the larger movement.
  float dd;
  if (m_mode != modes::examine) {
    dd = -dy;
  } else {
    dd = std::fabs(dx) > std::fabs(dy) ? dx : -dy;
  }
  float factor = m_speed * dd;

  // Adjust speed based on distance.
  if (m_mode == modes::examine) {
    // Don't move over the point of interest.
    ReturnUnless(factor < 1.0f);

    z *= factor;
  } else {
    // Normalize the Z vector and make it faster
    z *= factor / length * 10.0f;
  }

  // Not going up
  if (m_mode == modes::walk) {
    if (m_current.up.y > m_current.up.z) {
      z.y = 0;
    } else {
      z.z = 0;
    }
  }

  m_current.eye += z;

  // In fly mode, the interest moves with us.
  if (m_mode != modes::examine) {
    m_current.ctr += z;
  }
}

void camera::update_camera_buffer() {
  SceneCamera camera = create_host_camera();

  m_camera_buffer->update_data(&camera, sizeof(SceneCamera));
}

SceneCamera camera::create_host_camera() {
  SceneCamera camera{};
  memset(&camera, 0, sizeof(SceneCamera));

  const auto& view = get_view_matrix();
  auto proj = glm::perspectiveRH_ZO(glm::radians(get_fov()), get_aspect_ratio(),
                                    0.001f, 100000.0f);
  proj[1][1] *= -1;
  camera.viewInverse = glm::inverse(view);
  camera.projInverse = glm::inverse(proj);

  // Focal is the interest point
  glm::vec3 eye, center, up;
  get_lookat(eye, center, up);
  camera.focalDist = glm::length(center - eye);
  camera.nbLights = static_cast<uint32_t>(m_lights_count);
  return camera;
}

void camera::print_camera_angles() {
#if PRINT_CAMERA_ANGLES
  vec3 forward = glm::normalize(m_current.ctr - m_current.eye);
  float pitch = glm::degrees(glm::asin(-forward.y)); // vertical angle (Y up)
  float yaw = glm::degrees(glm::atan(forward.x, -forward.z)); // horizontal angle (Z forward)

  WUNDER_INFO_TAG("Camera", "{0} {1} {2}", pitch, yaw, m_current.fov);
#endif
}

}  // namespace wunder
