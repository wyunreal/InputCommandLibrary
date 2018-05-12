#include "Input.h"

Input input(100);

void commandWithParams(CommandParam** params, Stream* response) {
  // do command business here and then fullfill the command response:
  response->print("command 1: ");
  response->print(params[0]->asLongInt());
  response->print(" ");
  response->print(params[1]->asFloat());
  response->print(" ");
  response->println(params[2]->asString());
}

void commandWithNoParams(CommandParam** params, Stream* response) {
  // do command business here and then fullfill the command response:
  response->println("command 2");
}

const InputCommand commandDefinitions[] PROGMEM = defineCommands(
  command("com1", 3, &commandWithParams),
  command("com2", 0, &commandWithNoParams)
);

void setup() {
  // initialize input command reader with main Serial at 9600 bauds
  input.begin(9600, commandDefinitions);
}

void loop() {
  // your sketch can do here its main tasks
}

