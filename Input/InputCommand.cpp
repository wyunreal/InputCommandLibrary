#include "InputCommand.h"

InputCommand::InputCommand(char* aPattern, int aParamsCount, void (*aCommandFunction)(InputCommandParam** params)) {
  pattern = aPattern;
  paramsCount = aParamsCount;
  commandFunction = aCommandFunction;
}
