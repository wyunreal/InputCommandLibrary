#include "Input.h"
#include <Arduino.h>

struct SerialRuntime
{
  char commandsSeparator = 0;
  const InputCommand *commandDefinitions;
  int commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;

  int inputBufferIndex = 0;
  char *serialCommandBuffer;
};

struct SerialRuntimes
{
  SerialRuntime *serial0;
  SerialRuntime *serial1;
  SerialRuntime *serial2;
  SerialRuntime *serial3;
};

SerialRuntimes runtimes;

InputCommand currentCommandDefinition;
CommandParams paramsReader;
char *commandParams[INPUT_COMMAND_MAX_PARAMS];

char *CommandParams::getParamAsString(byte paramIndex)
{
  return commandParams[paramIndex];
}

int CommandParams::getParamAsInt(byte paramIndex)
{
  return atoi(commandParams[paramIndex]);
}

long CommandParams::getParamAsLongInt(byte paramIndex)
{
  return atol(commandParams[paramIndex]);
}

float CommandParams::getParamAsFloat(byte paramIndex)
{
  return atof(commandParams[paramIndex]);
}

SerialRuntime *getRuntime(SerialId serialId, bool init = false)
{
  switch (serialId)
  {
  case SERIAL_ID_0:
    if (init)
    {
      runtimes.serial0 = new SerialRuntime();
    }
    return runtimes.serial0;
  case SERIAL_ID_1:
    if (init)
    {
      runtimes.serial1 = new SerialRuntime();
    }
    return runtimes.serial1;
  case SERIAL_ID_2:
    if (init)
    {
      runtimes.serial2 = new SerialRuntime();
    }
    return runtimes.serial2;
  case SERIAL_ID_3:
    if (init)
    {
      runtimes.serial3 = new SerialRuntime();
    }
    return runtimes.serial3;
  }
  if (init)
  {
    runtimes.serial0 = new SerialRuntime();
  }
  return runtimes.serial0;
}

HardwareSerial *getHardwareSerialInstance(SerialId serialId)
{
  switch (serialId)
  {
  case SERIAL_ID_0:
    return &Serial;
  case SERIAL_ID_1:
    return &Serial1;
  case SERIAL_ID_2:
    return &Serial2;
  case SERIAL_ID_3:
    return &Serial3;
  }
  return &Serial;
}

Input::Input()
{
  serialId = SERIAL_ID_0;
  SerialRuntime *runtime = getRuntime(serialId, true);
  runtime->commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;
  runtime->serialCommandBuffer = new char[runtime->commandsMaxLength];
  memset(runtime->serialCommandBuffer, 0, runtime->commandsMaxLength);
}

Input::Input(SerialId aSerialId, int aCommandsMaxLength)
{
  serialId = aSerialId;
  SerialRuntime *runtime = getRuntime(serialId, true);
  runtime->commandsMaxLength = aCommandsMaxLength;
  runtime->serialCommandBuffer = new char[runtime->commandsMaxLength];
  memset(runtime->serialCommandBuffer, 0, runtime->commandsMaxLength);
}

Input::~Input()
{
  SerialRuntime *runtime = getRuntime(serialId);
  delete[] runtime->serialCommandBuffer;
  delete runtime;
}

void Input::begin(long baud, const InputCommand *aCommandDefinitions)
{
  SerialRuntime *runtime = getRuntime(serialId);
  runtime->commandsSeparator = 0;
  runtime->commandDefinitions = aCommandDefinitions;
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->begin(baud);
}

void Input::begin(long baud, char multiCommandSeparator, const InputCommand *aCommandDefinitions)
{
  SerialRuntime *runtime = getRuntime(serialId);
  runtime->commandsSeparator = multiCommandSeparator;
  runtime->commandDefinitions = aCommandDefinitions;
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->begin(baud);
}

HardwareSerial *Input::getSerialInterface()
{
  return getHardwareSerialInstance(serialId);
}

void Input::end()
{
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->end();
}

bool findCommandDefinition(char *opCodeString, SerialRuntime *runtime)
{
  int definitionIndex = 0;
  int found = false;
  while (true)
  {
    memcpy_P(&currentCommandDefinition, &runtime->commandDefinitions[definitionIndex], sizeof currentCommandDefinition);
    if (currentCommandDefinition.paramsCount == -1)
    {
      break;
    }
    if (strcmp(currentCommandDefinition.pattern, opCodeString) == 0)
    {
      found = true;
      break;
    }
    definitionIndex++;
  };

  return found;
}

bool parseCommand(SerialRuntime *runtime)
{
  char *token = strtok(runtime->serialCommandBuffer, " ");
  if (token != NULL)
  {
    if (findCommandDefinition(token, runtime))
    {
      token = strtok(NULL, " ");
      int paramIndex = 0;
      while (token != 0 && paramIndex < currentCommandDefinition.paramsCount && paramIndex < INPUT_COMMAND_MAX_PARAMS)
      {
        if (token[0] == '"')
        {
          token[strlen(token)] = ' ';
          token = strtok(token, "\"");
        }
        else if (token[0] == '\'')
        {
          token[strlen(token)] = ' ';
          token = strtok(token, "'");
        }
        commandParams[paramIndex++] = token;
        token = strtok(NULL, " ");
      }

      if (paramIndex == currentCommandDefinition.paramsCount)
      {
        return true;
      }
    }
  }

  return false;
}

bool processInputChar(char inChar, SerialRuntime *runtime, HardwareSerial &serial)
{
  if (runtime->inputBufferIndex >= runtime->commandsMaxLength - 1)
  {
    runtime->inputBufferIndex = 0;
    return true;
  }
  else
  {
    if (inChar != 13 && inChar != 10 && inChar != runtime->commandsSeparator)
    {
      runtime->serialCommandBuffer[runtime->inputBufferIndex++] = inChar;
    }
    else
    {
      runtime->serialCommandBuffer[runtime->inputBufferIndex++] = 0;
      bool commandParsed = parseCommand(runtime);
      if (commandParsed)
      {
        currentCommandDefinition.commandFunction(paramsReader, serial);
        memset(runtime->serialCommandBuffer, 0, runtime->commandsMaxLength);
      }
      runtime->inputBufferIndex = 0;
    }
    return false;
  }
}

void handleSerialEvent(SerialRuntime *runtime, HardwareSerial &serialInstance)
{
  bool bufferIsFull;
  while (serialInstance.available())
  {
    char inChar = (char)serialInstance.read();
    bufferIsFull = processInputChar(inChar, runtime, serialInstance);
    if (bufferIsFull || inChar == 13)
    {
      return;
    }
  }
}

void serialEvent()
{
  handleSerialEvent(runtimes.serial0, Serial);
}

void serialEvent1()
{
  handleSerialEvent(runtimes.serial1, Serial1);
}

void serialEvent2()
{
  handleSerialEvent(runtimes.serial2, Serial2);
}

void serialEvent3()
{
  handleSerialEvent(runtimes.serial3, Serial3);
}