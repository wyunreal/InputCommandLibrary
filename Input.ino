#include "Input.h"

char buffer[30];

Input input(buffer, 30);

void commandWithNoParams(CommandParams &params, ResponseWriter &response)
{
  // do command business here and then fullfill the command response:
  response.println("command 2");
  response.print("param: ");
  response.println(params.getParamAsLongInt(0));
  response.print("request id: ");
  response.println(params.getRequestId());
}

const InputCommand commandDefinitions[] PROGMEM = defineCommands(
    command("com", 1, &commandWithNoParams));

void setup()
{
  // initialize input command reader with main Serial at 9600 bauds, specifying a custom response printer
  input.withRequestId().address("example").begin(9600, '|', commandDefinitions);
}

void loop()
{
  // your sketch can do here its main tasks
}
