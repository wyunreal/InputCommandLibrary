#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "Arduino.h"

#define OP_CODE_MAX_LEN 20
#define INPUT_COMMAND_MAX_PARAMS 5
#define DEFAULT_COMMANDS_MAX_LENGTH 20

enum SerialId
{
  SERIAL_ID_0 = 0,
  SERIAL_ID_1 = 1,
  SERIAL_ID_2 = 2,
  SERIAL_ID_3 = 3
};

class CommandParams
{
public:
  char *getParamAsString(byte paramIndex);
  int getParamAsInt(byte paramIndex);
  long getParamAsLongInt(byte paramIndex);
  float getParamAsFloat(byte paramIndex);
};

struct InputCommand
{
  char pattern[OP_CODE_MAX_LEN];
  int paramsCount;
  void (*commandFunction)(CommandParams &params, Stream &response);
};

#define defineCommands(...)                    \
  {                                            \
    __VA_ARGS__, InputCommand { "", -1, NULL } \
  }
#define command(X, Y, Z) \
  InputCommand { X, Y, Z }

class Input
{
public:
  Input();
  Input(SerialId aSerialId, int aCommandsMaxLength);
  ~Input();
  void begin(long baud, const InputCommand *aCommandDefinitions);
  void begin(long baud, char multiCommandSeparator, const InputCommand *aCommandDefinitions);
  HardwareSerial *getSerialInterface();
  void end();

private:
  SerialId serialId;
};

#endif
