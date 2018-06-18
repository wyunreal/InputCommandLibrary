#include "Input.h"
#include <Arduino.h>

const InputCommand* commandDefinitions;
InputCommand currentCommandDefinition;
CommandParams paramsReader;
int commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;
int inputBufferIndex = 0;
char* serialCommandBuffer;
char* commandParams[INPUT_COMMAND_MAX_PARAMS];

char* CommandParams::getParamAsString(byte paramIndex) {
  return commandParams[paramIndex];
}

int CommandParams::getParamAsInt(byte paramIndex) {
  return atoi(commandParams[paramIndex]);
}

long CommandParams::getParamAsLongInt(byte paramIndex) {
  return atol(commandParams[paramIndex]);
}

float CommandParams::getParamAsFloat(byte paramIndex) {
  return atof(commandParams[paramIndex]);
}

Input::Input() {
  commandsMaxLength = DEFAULT_COMMANDS_MAX_LENGTH;
  serialCommandBuffer = new char[commandsMaxLength];
  memset(serialCommandBuffer, 0, commandsMaxLength);
}

Input::Input(int aCommandsMaxLength) {
  commandsMaxLength = aCommandsMaxLength;
  serialCommandBuffer = new char[commandsMaxLength];
  memset(serialCommandBuffer, 0, commandsMaxLength);
}

Input::~Input() {
  delete[] serialCommandBuffer;
}

void Input::begin(int baud, const InputCommand* aCommandDefinitions) {
  commandDefinitions = aCommandDefinitions;
  Serial.begin(baud);
}

void Input::end() {
  Serial.end();
}

bool findCommandDefinition(char* opCodeString) {
  int definitionIndex = 0;
  int found = false;
  while (true) {
    memcpy_P(&currentCommandDefinition, &commandDefinitions[definitionIndex], sizeof currentCommandDefinition);
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

bool parseCommand() {
  char* token = strtok (serialCommandBuffer, " ");
  if (token != NULL) {
    if (findCommandDefinition(token)) {
      token = strtok (NULL, " ");
      int paramIndex = 0;
      while (token != 0 && paramIndex < currentCommandDefinition.paramsCount && paramIndex < INPUT_COMMAND_MAX_PARAMS)
      {
        if (token[0] == '"') {
          token[strlen(token)] = ' ';
          token = strtok (token, "\"");
        } else if (token[0] == '\'') {
          token[strlen(token)] = ' ';
          token = strtok (token, "'");
        }
        commandParams[paramIndex++] = token;
        token = strtok (NULL, " ");
      }

      if (paramIndex == currentCommandDefinition.paramsCount) {
        return true;
      }
    }
  }

  return false;
}

bool processInputChar(char inChar) {
  if (inputBufferIndex >= commandsMaxLength - 1) {
    inputBufferIndex = 0;
    return true;
  } else {
    if (inChar != 13 && inChar != 10) {
      serialCommandBuffer[inputBufferIndex++] = inChar;
    } else {
      serialCommandBuffer[inputBufferIndex++] = 0;
      bool commandParsed = parseCommand();
      if (commandParsed) {
        currentCommandDefinition.commandFunction(paramsReader, Serial);
        memset(serialCommandBuffer, 0, commandsMaxLength);
      }
      inputBufferIndex = 0;
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

