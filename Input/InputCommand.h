#ifndef INPUT_COMMAND_H
#define INPUT_COMMAND_H

#include "Arduino.h"
#include "CommandParam.h"

class InputCommand {
  public:
    const char* pattern;
    int paramsCount;
    void (*commandFunction)(CommandParam** params, Stream* response);
    InputCommand(const char* aPattern, int aParamsCount, void (*aCommandFunction)(CommandParam** params, Stream* response));
};

#endif
