#include "InputCommandParam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InputCommandParam::InputCommandParam(char* aValue) {
  value = new char[strlen(aValue) + 1];
  strcpy(value, aValue);
}

InputCommandParam::~InputCommandParam() {
  delete[] value;
}

char* InputCommandParam::asString() {
  return value;
}

int InputCommandParam::asInt() {
  return atoi(value);
}

float InputCommandParam::asFloat() {
  return atof(value);
}
