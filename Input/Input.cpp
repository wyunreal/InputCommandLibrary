#include "Input.h"
#include <Arduino.h>

#define DEFAULT_COMMANDS_MAX_LENGTH 20

struct InputCommandData {
  CommandParam* params[INPUT_COMMAND_MAX_PARAMS];
  int paramsCount;
  void (*commandFunction)(CommandParam** params, Stream* response);
  Stream* response;
};

InputCommand* _commandDefinitions;

InputCommand currentCommandDefinition;

int _commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;
int _inputBufferIndex = 0;
char* _serialCommandBuffer;

Input::Input() {
  _commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;
  _serialCommandBuffer = new char[_commandsMaxLength];
}

Input::Input(int commandsMaxLength) {
  _commandsMaxLength = commandsMaxLength;
  _serialCommandBuffer = new char[_commandsMaxLength];
}

Input::~Input() {
  delete[] _serialCommandBuffer;
}

void Input::begin(int baud, InputCommand* aCommandDefinitions) {
  _commandDefinitions = aCommandDefinitions;
  Serial.begin(baud);
}

bool findCommandDefinition(char* opCodeString) {
  int definitionIndex = 0;
  int found = false;
  while (true) {
    memcpy_P (&currentCommandDefinition, &_commandDefinitions[definitionIndex], sizeof currentCommandDefinition);
    if (currentCommandDefinition.paramsCount == -1) {
      break;
    }
    if (strcmp(currentCommandDefinition.pattern, opCodeString) == 0) {
      found = true;
      break;
    }
    definitionIndex++;
  };

  return found;
}

void freeCommand(InputCommandData* command) {
  for (int i = 0; i < command->paramsCount; i++) {
    delete command->params[i];
  }
  delete command;
}

InputCommandData* createCommand(char* commandString) {
  char* token = strtok (commandString, " ");
  if (token != NULL) {
    if (findCommandDefinition(token)) {
      InputCommandData* command = new InputCommandData;

      command->commandFunction = currentCommandDefinition.commandFunction;
      command->response = &Serial;

      token = strtok (NULL, " ");
      int paramIndex = 0;
      while (token != 0 && paramIndex < currentCommandDefinition.paramsCount && paramIndex < INPUT_COMMAND_MAX_PARAMS)
      {
        command->params[paramIndex] = new CommandParam(token);

        paramIndex++;
        token = strtok (NULL, " ");
      }
      command->paramsCount = paramIndex;

      if (command->paramsCount == currentCommandDefinition.paramsCount) {
        return command;
      } else {
        freeCommand(command);
      }
    }
  }

  return NULL;
}

bool processInputChar(char inChar) {
  if (_inputBufferIndex >= _commandsMaxLength - 1) {
    _inputBufferIndex = 0;
    return true;
  } else {
    if (inChar != 13 && inChar != 10) {
      _serialCommandBuffer[_inputBufferIndex++] = inChar;
    } else {
      _serialCommandBuffer[_inputBufferIndex++] = 0;
      InputCommandData* command = createCommand(_serialCommandBuffer);
      if (command != NULL) {
        command->commandFunction(command->params, command->response);
        freeCommand(command);
      }
      _inputBufferIndex = 0;
    }
    return false;
  }
}

void serialEvent() {
  bool bufferIsFull;
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    bufferIsFull = processInputChar(inChar);
    if (bufferIsFull || inChar == 13) {
      return;
    }
  }
}

