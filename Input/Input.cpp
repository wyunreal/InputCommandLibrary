#include "Input.h"
#include <Arduino.h>

#define DEFAULT_COMMANDS_MAX_LENGTH 20

struct InputCommandData {
  char* pattern;
  CommandParam* params[INPUT_COMMAND_MAX_PARAMS];
  int paramsCount;
  void (*commandFunction)(CommandParam** params, Stream* response);
  Stream* response;
};

InputCommand** _commandDefinitions;

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

void Input::begin(int baud, InputCommand** aCommandDefinitions) {
  _commandDefinitions = aCommandDefinitions;
  Serial.begin(baud);
}

InputCommand* findCommandDefinition(char* opCodeString) {
  int definitionIndex = 0;
  InputCommand* definition = NULL;
  while (!definition && _commandDefinitions[definitionIndex]) {
    if (strcmp(_commandDefinitions[definitionIndex]->pattern, opCodeString) == 0) {
      definition = _commandDefinitions[definitionIndex];
    }
    definitionIndex++;
  };

  return definition;
}

void freeCommand(InputCommandData* command) {
  delete[] command->pattern;
  for (int i = 0; i < command->paramsCount; i++) {
    delete command->params[i];
  }
  delete command;
}

InputCommandData* createCommand(char* commandString) {
  char* token = strtok (commandString, " ");
  if (token != NULL) {
    InputCommand* commandDefinition = findCommandDefinition(token);
    if (commandDefinition) {

      InputCommandData* command = new InputCommandData;
      command->pattern = new char[strlen(token) + 1];
      strcpy(command->pattern, token);

      command->commandFunction = commandDefinition->commandFunction;
      command->response = &Serial;

      token = strtok (NULL, " ");
      int paramIndex = 0;
      while (token != 0 && paramIndex < commandDefinition->paramsCount && paramIndex < INPUT_COMMAND_MAX_PARAMS)
      {
        command->params[paramIndex] = new CommandParam(token);

        paramIndex++;
        token = strtok (NULL, " ");
      }
      command->paramsCount = paramIndex;

      if (command->paramsCount == commandDefinition->paramsCount) {
        return command;
      } else {
        freeCommand(command);
      }
    }
  }

  return NULL;
}

void Input::trigger(const char* commandLine) {
  strcpy(_serialCommandBuffer, commandLine);
  InputCommandData* command = createCommand(_serialCommandBuffer);
  if (command != NULL) {
    command->commandFunction(command->params, command->response);
    freeCommand(command);
  }
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

