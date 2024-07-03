#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#include "application.h"

int main(int argc, char **argv) {
  auto app = wunder::create_application();

  app->init();

  app->run();

  delete app;

  return 0;
}

#endif
