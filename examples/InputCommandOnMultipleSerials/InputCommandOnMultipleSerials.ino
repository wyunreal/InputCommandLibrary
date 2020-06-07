#include <Input.h>

char usbBuffer[30];
char serial1Buffer[40];

Input usbInput(usbBuffer, 30);
Input serial1Input(serial1Buffer, 40);

void commandWithParams(CommandParams &params, ResponseWriter &response)
{
    // do command business here and then fullfill the command response:
    response.print("command 1: ");
    response.print(params.getParamAsLongInt(0));
    response.print(" ");
    response.print(params.getParamAsFloat(1));
    response.print(" ");
    response.println(params.getParamAsString(2));
}

void commandWithNoParams(CommandParams &params, ResponseWriter &response)
{
    // do command business here and then fullfill the command response:
    response.println("command 2");
}

const InputCommand usbCommandDefinitions[] PROGMEM = defineCommands(
    command("com1", 3, &commandWithParams),
    command("com2", 0, &commandWithNoParams));

const InputCommand serial1CommandDefinitions[] PROGMEM = defineCommands(
    command("com1", 3, &commandWithParams));

void setup()
{
    // initialize input command reader with main Serial at 9600 bauds
    usbInput.begin(9600, usbCommandDefinitions);
    serial1Input.port(SERIAL_ID_1).begin(9600, serial1CommandDefinitions);
}

void loop()
{
    // your sketch can do here its main tasks
}