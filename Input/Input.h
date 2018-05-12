#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "CommandParam.h"
#include "Arduino.h"

#define OP_CODE_MAX_LEN 20

struct InputCommand {
  char pattern[OP_CODE_MAX_LEN];
  int paramsCount;
  void (*commandFunction)(CommandParam** params, Stream* response);
};

#define defineCommands(...)  {__VA_ARGS__,InputCommand {NULL, -1, NULL}}
#define command(X,Y,Z)  InputCommand {X, Y, Z}

#define INPUT_COMMAND_MAX_PARAMS 3

class Input {
  public:
    Input();
    Input(int commandsMaxLength);
    ~Input();
    void begin(int baud, InputCommand* aCommandDefinitions);
};

#endif
