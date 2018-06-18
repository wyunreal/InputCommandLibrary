#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "Arduino.h"

#define OP_CODE_MAX_LEN 20
#define INPUT_COMMAND_MAX_PARAMS 5
#define DEFAULT_COMMANDS_MAX_LENGTH 20

class CommandParams {
  public:
    char* getParamAsString(byte paramIndex);
    int getParamAsInt(byte paramIndex);
    long getParamAsLongInt(byte paramIndex);
    float getParamAsFloat(byte paramIndex);
};

struct InputCommand {
  char pattern[OP_CODE_MAX_LEN];
  int paramsCount;
  void (*commandFunction)(CommandParams &params, Stream &response);
};

#define defineCommands(...)  {__VA_ARGS__,InputCommand {"", -1, NULL}}
#define command(X,Y,Z)  InputCommand {X, Y, Z}

class Input {
  public:
    Input();
    Input(int aCommandsMaxLength);
    ~Input();
    void begin(int baud, const InputCommand* aCommandDefinitions);
    void end();
};

#endif
