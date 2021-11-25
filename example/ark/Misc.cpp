#include "Misc.h"

Misc::Misc() {
}

POTTO_ERROR Misc::FinalConstruct() {
  return POTTO_E_OK;
}

void Misc::FinalRelease() {
}

void Misc::Info(const char* msg) {
  printf("Info: %s\n", msg);
}

void Misc::Warn(const char* msg) {
  printf("Warn: %s\n", msg);
}

void Misc::Error(const char* msg) {
  printf("Error: %s\n", msg);
}

void Misc::Fatal(const char* msg) {
  printf("Fatal: %s\n", msg);
}

bool Misc::Play(const char* file) {
  printf("Now playing %s\n", file);
  return true;
}
