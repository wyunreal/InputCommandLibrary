#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "InputCommandParam.h"

#define INPUT_COMMAND_MAX_PARAMS 3

class InputCommandDefinition {
  public:
    char* pattern;
    int paramsCount;
    void (*commandFunction)(InputCommandParam** params);
    InputCommandDefinition(char* aPattern, int aParamsCount, void (*aCommandFunction)(InputCommandParam** params));
};

class InputCommandReader {
  public:
    void begin(int baud, InputCommandDefinition** aCommandDefinitions);
};

#endif
