#ifndef INPUT_COMMAND_READER_H
#define INPUT_COMMAND_READER_H

#include "Arduino.h"

#define OP_CODE_MAX_LEN 20
#define INPUT_COMMAND_MAX_PARAMS 10
#define DEFAULT_COMMANDS_MAX_LENGTH 20

typedef void (*InputBroadcastHandler)(char *command);

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
  int getRequestId();
  char *getParamAsString(byte paramIndex);
  int getParamAsInt(byte paramIndex);
  long getParamAsLongInt(byte paramIndex);
  float getParamAsFloat(byte paramIndex);
};

class ResponseWriter: public Print
{
public:
  ResponseWriter();

  Print *getStream();
  void setStream(Print *aPrinter);
  virtual void setAddressFromParams(CommandParams &params, int paramsCount) {}
  void setAddress(char *anAddress) { address = anAddress; }
  char *getAddress() { return address; }
  void setRequestId(int aRequestId) { requestId = aRequestId; }
  int getRequestId() { return requestId; }
  void setConnectionId(int aConnectionId) { connectionId = aConnectionId; }
  int getConnectionId() { return connectionId; }

  virtual size_t write(uint8_t);

  virtual size_t print(const __FlashStringHelper *);
  virtual size_t print(const char[]);
  virtual size_t print(char);
  virtual size_t print(unsigned char, int = DEC);
  virtual size_t print(int, int = DEC);
  virtual size_t print(unsigned int, int = DEC);
  virtual size_t print(long, int = DEC);
  virtual size_t print(unsigned long, int = DEC);
  virtual size_t print(double, int = 2);

  virtual size_t println(const __FlashStringHelper *);
  virtual size_t println(const char[]);
  virtual size_t println(char);
  virtual size_t println(unsigned char, int = DEC);
  virtual size_t println(int, int = DEC);
  virtual size_t println(unsigned int, int = DEC);
  virtual size_t println(long, int = DEC);
  virtual size_t println(unsigned long, int = DEC);
  virtual size_t println(double, int = 2);

  virtual size_t println(void);

  bool isNewLine();

protected:
  bool newLineWritten;

private:
  Print *printer;
  char *address;
  uint8_t connectionId;
  int requestId;
};

struct InputCommand
{
  char pattern[OP_CODE_MAX_LEN];
  int paramsCount;
  void (*commandFunction)(CommandParams &params, ResponseWriter &response);
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
  Input(char *aBuffer, int aBufferLen);
  ~Input();

  Input &port(SerialId aSerialId);
  Input &address(char *anAddress);
  Input &address(char *anAddress, char *aBroadcastAddress);
  Input &broadcastHandler(InputBroadcastHandler aBroadcastHandler);
  Input &responseWriter(ResponseWriter *aWriter);
  Input &isSlave();
  Input &withRequestId();

  void begin(long baud, const InputCommand *aCommandDefinitions);
  void begin(long baud, char multiCommandSeparator, const InputCommand *aCommandDefinitions);
  ResponseWriter &getSerialInterface();
  void end();

private:
  SerialId serialId;
  char *addressId;
  char *broadcastAddressId;
  char *buffer;
  int bufferLen;
  bool slave;
  bool useRequestId;
  ResponseWriter defaultWriter;
  ResponseWriter *respWriter;
  InputBroadcastHandler bcastHandler;
};

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
  bool isSlave = false;
  bool withRequestId = false;
  bool commandIsBroadcast;
  int commandLen = 0;
  InputBroadcastHandler broadcastHandler;
};

bool processInputChar(char inChar, SerialRuntime *runtime, Print *serial = NULL);

#endif