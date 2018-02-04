#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "InputCommand.h"

#define INPUT_COMMAND_MAX_PARAMS 3

class InputCommandReader {
  public:
    void begin(int baud, InputCommand** aCommandDefinitions);
};

#endif
