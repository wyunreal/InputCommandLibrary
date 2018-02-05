#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "InputCommand.h"

#define defineCommands(...)  {__VA_ARGS__,NULL}
#define command(X,Y,Z)  new InputCommand(X, Y, Z)

#define INPUT_COMMAND_MAX_PARAMS 3

class Input {
  public:
    void begin(int baud, InputCommand** aCommandDefinitions);
};

#endif