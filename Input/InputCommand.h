#ifndef INPUT_COMMAND_H
#define INPUT_COMMAND_H

#include "Arduino.h"
#include "CommandParam.h"
#include "CommandResponse.h"

class InputCommand {
  public:
    char* pattern;
    int paramsCount;
    void (*commandFunction)(CommandParam** params);
    InputCommand(char* aPattern, int aParamsCount, void (*aCommandFunction)(CommandParam** params, Stream* response));
};

#endif
