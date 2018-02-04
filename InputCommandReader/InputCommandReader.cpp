#include "InputCommandReader.h"
#include <Arduino.h>

#define INPUT_COMMAND_MAX_LEN 20

struct InputCommand {
  char* pattern;
  InputCommandParam* params[INPUT_COMMAND_MAX_PARAMS];
  int paramsCount;
  void (*commandFunction)(InputCommandParam** params);
};

InputCommandDefinition** _commandDefinitions;

int _inputBufferIndex = 0;
char _serialCommandBuffer[INPUT_COMMAND_MAX_LEN];

InputCommandDefinition::InputCommandDefinition(char* aPattern, int aParamsCount, void (*aCommandFunction)(InputCommandParam** params)) {
  pattern = aPattern;
  paramsCount = aParamsCount;
  commandFunction = aCommandFunction;
}

void InputCommandReader::begin(int baud, InputCommandDefinition** aCommandDefinitions) {
  _commandDefinitions = aCommandDefinitions;
  Serial.begin(baud);
}

InputCommandDefinition* findCommandDefinition(char* opCodeString) {
  int definitionIndex = 0;
  InputCommandDefinition* definition = NULL;
  while (!definition && _commandDefinitions[definitionIndex]) {
    if (strcmp(_commandDefinitions[definitionIndex]->pattern, opCodeString) == 0) {
      definition = _commandDefinitions[definitionIndex];
    }
    definitionIndex++;
  };

  return definition;
}

void freeCommand(InputCommand* command) {
  delete[] command->pattern;
  for (int i = 0; i < command->paramsCount; i++) {
    delete command->params[i];
  }
  delete command;
}

InputCommand* createCommand(char* commandString) {
  char* token = strtok (commandString, " ");
  if (token != NULL) {
    InputCommandDefinition* commandDefinition = findCommandDefinition(token);
    if (commandDefinition) {

      InputCommand* command = new InputCommand;
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
    InputCommand* command = createCommand(_serialCommandBuffer);
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

