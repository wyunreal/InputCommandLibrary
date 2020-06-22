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

class ResponseWriter
{
public:
  ResponseWriter();

  Print *getStream();
  void setStream(Print *aPrinter);
  virtual void setAddressFromParams(CommandParams &params, int paramsCount) {}
  void setAddress(char *anAddress) { address = anAddress; }
  char *getAddress() { return address; }

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

protected:
  bool isNewLine();

private:
  Print *printer;
  bool newLineWritten;
  char *address;
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
  Input &responseWriter(ResponseWriter *aWriter);
  Input &isSlave();

  void begin(long baud, const InputCommand *aCommandDefinitions);
  void begin(long baud, char multiCommandSeparator, const InputCommand *aCommandDefinitions);
  ResponseWriter &getSerialInterface();
  void end();

private:
  SerialId serialId;
  char *addressId;
  char *buffer;
  int bufferLen;
  bool slave;
  ResponseWriter defaultWriter;
  ResponseWriter *respWriter;
};

#endif
