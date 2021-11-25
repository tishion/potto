#include "Misc.h"

Misc::Misc() {
}

POTTO_ERROR Misc::FinalConstruct() {
  return POTTO_E_OK;
}

void Misc::FinalRelease() {
}

void Misc::Info(char* msg) {
  printf("Info: %s\r\n", msg);
}

void Misc::Warn(char* msg) {
  printf("Warn: %s\r\n", msg);
}

void Misc::Error(char* msg) {
  printf("Error: %s\r\n", msg);
}

void Misc::Fatal(char* msg) {
  printf("Fatal: %s\r\n", msg);
}

bool Misc::Play(char* file) {
  printf("Now playing %s\r\n", file);
  return true;
}
