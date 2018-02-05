#include "Input.h"

Input input;

void commandWithParams(InputCommandParam** params) {
  Serial.print("command 1: ");
  Serial.print(params[0]->asInt());
  Serial.print(" ");
  Serial.print(params[1]->asFloat());
  Serial.print(" ");
  Serial.println(params[2]->asString());
}

void commandWithNoParams(InputCommandParam** params) {
  Serial.println("command 2");
}

InputCommand* commandDefinitions[] = defineCommands(
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

