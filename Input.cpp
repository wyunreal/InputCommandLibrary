#include "Input.h"
#include <Arduino.h>

struct SerialRuntime
{
  char *addressId = NULL;
  char *broadcastAddressId = NULL;
  char commandsSeparator = 0;
  const InputCommand *commandDefinitions;
  int commandsMaxLength;
  int inputBufferIndex = 0;
  char *serialCommandBuffer;
  ResponseWriter *respWriter;
  bool isSlave;
  bool commandIsBroadcast;
  int commandLen = 0;
  InputBroadcastHandler broadcastHandler;
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

bool isLineBreak(uint8_t value)
{
  return value == 13 || value == 10;
}

ResponseWriter::ResponseWriter()
{
  printer = NULL;
  newLineWritten = true;
}

Print *ResponseWriter::getStream()
{
  return printer;
}

void ResponseWriter::setStream(Print *aPrinter)
{
  printer = aPrinter;
}

bool ResponseWriter::isNewLine()
{
  bool result = newLineWritten;
  newLineWritten = false;
  return result;
}

size_t ResponseWriter::write(uint8_t value)
{
  if (isLineBreak(value))
  {
    newLineWritten = true;
  }
  return printer->write(value);
}

size_t ResponseWriter::print(const __FlashStringHelper *value)
{
  return printer->print(value);
}

size_t ResponseWriter::print(const char value[])
{
  for (int i = 0; i < strlen(value); i++)
  {
    if (isLineBreak(value[i]))
    {
      newLineWritten = true;
    }
  }
  return printer->print(value);
}

size_t ResponseWriter::print(char value)
{
  if (isLineBreak(value))
  {
    newLineWritten = true;
  }
  return printer->print(value);
}

size_t ResponseWriter::print(unsigned char value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::print(int value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::print(unsigned int value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::print(long value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::print(unsigned long value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::print(double value, int base)
{
  return printer->print(value, base);
}

size_t ResponseWriter::println(const __FlashStringHelper *value)
{
  newLineWritten = true;
  return printer->println(value);
}

size_t ResponseWriter::println(const char value[])
{
  newLineWritten = true;
  return printer->println(value);
}

size_t ResponseWriter::println(char value)
{
  newLineWritten = true;
  return printer->println(value);
}

size_t ResponseWriter::println(unsigned char value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(int value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(unsigned int value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(long value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(unsigned long value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(double value, int base)
{
  newLineWritten = true;
  return printer->println(value, base);
}

size_t ResponseWriter::println(void)
{
  newLineWritten = true;
  return printer->println();
}

Input::Input(char *aBuffer, int aBufferLen)
{
  buffer = aBuffer;
  bufferLen = aBufferLen;
  serialId = SERIAL_ID_0;
  addressId = NULL;
  broadcastAddressId = NULL;
  respWriter = &defaultWriter;
  slave = false;
}

Input::~Input()
{
  SerialRuntime *runtime = getRuntime(serialId);
  delete runtime;
}

Input &Input::port(SerialId aSerialId)
{
  serialId = aSerialId;
  return *this;
}

Input &Input::address(char *anAddress)
{
  addressId = anAddress;
  broadcastAddressId = NULL;
  return *this;
}

Input &Input::address(char *anAddress, char *aBroadcastAddress)
{
  addressId = anAddress;
  broadcastAddressId = aBroadcastAddress;
  return *this;
}

Input &Input::broadcastHandler(InputBroadcastHandler aBroadcastHandler)
{
  bcastHandler = aBroadcastHandler;
  return *this;
}

Input &Input::responseWriter(ResponseWriter *aWriter)
{
  respWriter = aWriter;
  return *this;
}

Input &Input::isSlave()
{
  slave = true;
  return *this;
}

SerialRuntime *InitRuntime(SerialId serialId, char *addressId, char *broadcastAddressId, char *buffer, int bufferLen, ResponseWriter *aWriter, bool isSlave, InputBroadcastHandler broadcastHandler)
{
  SerialRuntime *runtime = getRuntime(serialId, true);
  runtime->addressId = addressId;
  runtime->broadcastAddressId = broadcastAddressId;
  runtime->commandsMaxLength = bufferLen - 1;
  runtime->serialCommandBuffer = buffer;
  runtime->respWriter = aWriter;
  memset(runtime->serialCommandBuffer, 0, runtime->commandsMaxLength + 1);
  runtime->isSlave = isSlave;
  runtime->commandIsBroadcast = false;
  runtime->broadcastHandler = broadcastHandler;

  return runtime;
}

void Input::begin(long baud, const InputCommand *aCommandDefinitions)
{
  SerialRuntime *runtime = InitRuntime(serialId, addressId, broadcastAddressId, buffer, bufferLen, respWriter, slave, bcastHandler);

  runtime->commandsSeparator = 0;
  runtime->commandDefinitions = aCommandDefinitions;
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->begin(baud);
}

void Input::begin(long baud, char multiCommandSeparator, const InputCommand *aCommandDefinitions)
{
  SerialRuntime *runtime = InitRuntime(serialId, addressId, broadcastAddressId, buffer, bufferLen, respWriter, slave, bcastHandler);

  runtime->commandsSeparator = multiCommandSeparator;
  runtime->commandDefinitions = aCommandDefinitions;
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->begin(baud);
}

ResponseWriter &Input::getSerialInterface()
{
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  defaultWriter.setStream(serial);
  return defaultWriter;
}

void Input::end()
{
  HardwareSerial *serial = getHardwareSerialInstance(serialId);
  serial->end();
}

bool findCommandDefinition(char *opCodeString, SerialRuntime *runtime)
{
  int definitionIndex = 0;
  while (true)
  {
    memcpy_P(&currentCommandDefinition, &runtime->commandDefinitions[definitionIndex], sizeof currentCommandDefinition);
    if (currentCommandDefinition.paramsCount == -1)
    {
      return false;
    }
    if (strlen(currentCommandDefinition.pattern) == 0 || strcmp(currentCommandDefinition.pattern, opCodeString) == 0)
    {
      return true;
    }
    definitionIndex++;
  };

  return false;
}

void concatTokens(char *buffer, int bufferLen, int concatCount)
{
  for (int i = 0; i < bufferLen; i++)
  {
    if (buffer[i] == 0)
    {
      buffer[i] = ' ';
      if (--concatCount == 0)
      {
        return;
      }
    }
  }
}

bool parseCommand(SerialRuntime *runtime)
{
  runtime->commandLen = strlen(runtime->serialCommandBuffer);
  bool hasAddress = !runtime->isSlave && runtime->addressId != NULL && strlen(runtime->addressId) > 0;
  char *opcode = strtok(runtime->serialCommandBuffer, " ");
  int concatCount = 1;
  char *addr = NULL;

  runtime->commandIsBroadcast = false;
  if (opcode != NULL && hasAddress)
  {
    addr = strtok(NULL, " ");
    runtime->commandIsBroadcast = strcmp(addr, runtime->broadcastAddressId) == 0;
    concatCount = 2;
  }

  bool commandFound = false;
  bool definitionFound = false;

  if (opcode != NULL)
  {
    if (hasAddress && (addr == NULL || (strcmp(addr, runtime->addressId) != 0 && !runtime->commandIsBroadcast)))
    {
      definitionFound = findCommandDefinition(NULL, runtime);
    }
    else
    {
      definitionFound = commandFound = findCommandDefinition(opcode, runtime);
    }
  }

  if (commandFound && strlen(currentCommandDefinition.pattern) > 0)
  {
    char *token = strtok(NULL, " ");
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
  else if (definitionFound)
  {
    concatTokens(runtime->serialCommandBuffer, runtime->commandLen, concatCount);
    commandParams[0] = runtime->serialCommandBuffer;
    return true;
  }

  return false;
}

bool processInputChar(char inChar, SerialRuntime *runtime, HardwareSerial &serial)
{
  if (runtime->inputBufferIndex >= runtime->commandsMaxLength)
  {
    runtime->inputBufferIndex = 0;
    return true;
  }
  else
  {
    if (!isLineBreak(inChar) && inChar != runtime->commandsSeparator)
    {
      runtime->serialCommandBuffer[runtime->inputBufferIndex++] = inChar;
    }
    else
    {
      runtime->serialCommandBuffer[runtime->inputBufferIndex++] = 0;
      bool commandParsed = parseCommand(runtime);
      if (commandParsed)
      {
        runtime->respWriter->setStream(&serial);
        if (runtime->isSlave)
        {
          runtime->respWriter->setAddressFromParams(paramsReader, currentCommandDefinition.paramsCount);
        }
        currentCommandDefinition.commandFunction(paramsReader, *runtime->respWriter);
        if (runtime->commandIsBroadcast and runtime->broadcastHandler != NULL)
        {
          concatTokens(runtime->serialCommandBuffer, runtime->commandLen, 2);
          runtime->broadcastHandler(runtime->serialCommandBuffer);
        }
        memset(runtime->serialCommandBuffer, 0, runtime->commandsMaxLength + 1);
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
