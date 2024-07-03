#ifndef WUNDER_GRAPHIC_LAYER_ABSTRACTION_FACTORY_H
#define WUNDER_GRAPHIC_LAYER_ABSTRACTION_FACTORY_H

#include "core/wunder_memory.h"

namespace wunder {

struct renderer_properties;

class renderer_api;

class graphic_layer_abstraction_factory {
 protected:
  graphic_layer_abstraction_factory() = default;

 public:
  virtual ~graphic_layer_abstraction_factory() = default;

 public:
  static void create_instance(const renderer_properties &properties);
  [[nodiscard]] static graphic_layer_abstraction_factory &get_instance();

 public:
  [[nodiscard]] virtual const renderer_api &get_renderer_api() const = 0;

 protected:
  virtual void init_instance_internal(
      const renderer_properties &properties) = 0;

 private:
  void init_instance(const renderer_properties &properties);

 private:
  static unique_ptr<graphic_layer_abstraction_factory> s_instance;
};
}  // namespace wunder

#endif  // WUNDER_GRAPHIC_LAYER_ABSTRACTION_FACTORY_H
