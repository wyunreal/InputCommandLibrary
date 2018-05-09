#include "CommandParam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CommandParam::CommandParam(char* aValue) {
  value = new char[strlen(aValue) + 1];
  strcpy(value, aValue);
}

CommandParam::~CommandParam() {
  delete[] value;
}

char* CommandParam::asString() {
  return value;
}

int CommandParam::asInt() {
  return atoi(value);
}

long CommandParam::asLongInt() {
  return atol(value);
}

float CommandParam::asFloat() {
  return atof(value);
}
