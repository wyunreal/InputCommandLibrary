#include <Input.h>

Input input(100);

void commandWithParams(CommandParams &params, Stream &response) {
  // do command business here and then fullfill the command response:
  response.print("command 1: ");
  response.print(params.getParamAsLongInt(0));
  response.print(" ");
  response.print(params.getParamAsFloat(1));
  response.print(" ");
  response.println(params.getParamAsString(2));
}

void commandWithNoParams(CommandParams &params, Stream &response) {
  // do command business here and then fullfill the command response:
  response.println("command 2");
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
