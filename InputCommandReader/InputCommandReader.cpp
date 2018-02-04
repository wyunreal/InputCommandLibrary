#include "InputCommandReader.h"
#include <Arduino.h>

#define INPUT_COMMAND_MAX_LEN 20

struct InputCommandData {
  char* pattern;
  InputCommandParam* params[INPUT_COMMAND_MAX_PARAMS];
  int paramsCount;
  void (*commandFunction)(InputCommandParam** params);
};

InputCommand** _commandDefinitions;

int _inputBufferIndex = 0;
char _serialCommandBuffer[INPUT_COMMAND_MAX_LEN];

void InputCommandReader::begin(int baud, InputCommand** aCommandDefinitions) {
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
      
      token = strtok (NULL, " ");
      int paramIndex = 0;
      while (token != 0 && paramIndex < commandDefinition->paramsCount && paramIndex < INPUT_COMMAND_MAX_PARAMS)
      {
        command->params[paramIndex] = new InputCommandParam(token);

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

void processInputChar(char inChar) {
  if (inChar != 13) {
    _serialCommandBuffer[_inputBufferIndex++] = inChar;
  } else {
    _serialCommandBuffer[_inputBufferIndex++] = NULL;
    InputCommandData* command = createCommand(_serialCommandBuffer);
    if (command != NULL) {
      command->commandFunction(command->params);
      freeCommand(command);
    }
    _inputBufferIndex = 0;
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    processInputChar(inChar);
    if (inChar == 13) {
      return;
    }
  }
}

