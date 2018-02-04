#ifndef INPUT_COMMAND_H
#define INPUT_COMMAND_H

#include "InputCommandParam.h"

class InputCommand {
  public:
    char* pattern;
    int paramsCount;
    void (*commandFunction)(InputCommandParam** params);
    InputCommand(char* aPattern, int aParamsCount, void (*aCommandFunction)(InputCommandParam** params));
};

#endif
