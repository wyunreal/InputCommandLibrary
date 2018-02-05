#include "InputCommand.h"

InputCommand::InputCommand(char* aPattern, int aParamsCount, void (*aCommandFunction)(CommandParam** params, Stream* response)) {
  pattern = aPattern;
  paramsCount = aParamsCount;
  commandFunction = aCommandFunction;
}
