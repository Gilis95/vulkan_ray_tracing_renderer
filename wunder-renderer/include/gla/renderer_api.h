#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace wunder {
struct renderer_properties;

class renderer_api {
 public:
  virtual ~renderer_api();

 public:
  void init(const renderer_properties& properties);

 protected:
  virtual void init_internal(const renderer_properties& properties) = 0;
};
}  // namespace wunder
#endif
