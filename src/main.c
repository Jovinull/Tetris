#include "core/app.h"
#include "utils/log.h"

int main(void) {
  App app;
  if (!app_init(&app)) {
    log_error("Falha ao inicializar o app.");
    return 1;
  }
  app_run(&app);
  app_shutdown(&app);
  return 0;
}