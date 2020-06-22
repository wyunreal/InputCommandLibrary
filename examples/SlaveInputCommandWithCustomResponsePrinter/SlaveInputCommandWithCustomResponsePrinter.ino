#include <Input.h>

char buffer[30];

Input input(buffer, 30);

void commandWithParams(CommandParams &params, ResponseWriter &response)
{
    // do command business here and then fullfill the command response:
    response.print("command 1: ");
    response.print(params.getParamAsLongInt(1));
    response.print(" ");
    response.print(params.getParamAsFloat(2));
    response.print(" ");
    response.println(params.getParamAsString(3));
}

void commandWithNoParams(CommandParams &params, ResponseWriter &response)
{
    // do command business here and then fullfill the command response:
    response.println("command 2");
}

const InputCommand commandDefinitions[] PROGMEM = defineCommands(
    command("com1", 4, &commandWithParams),
    command("com2", 1, &commandWithNoParams));

class CustomWriter : public ResponseWriter
{
public:
    // Yout should override the setAddressFromParams method if you want the address to be autoimatically set by the library
    virtual void setAddressFromParams(CommandParams &params, int paramsCount)
    {
        setAddress(params.getParamAsString(0));
    }

    // You can override the Print method you need
    virtual size_t print(const char value[])
    {
        size_t size = ResponseWriter::print(value);
        size += ResponseWriter::print(", address: ");
        size += ResponseWriter::println(getAddress());
        return size;
    }
    virtual size_t println(const char value[])
    {
        size_t size = ResponseWriter::print(value);
        size += ResponseWriter::print(", address: ");
        size += ResponseWriter::println(getAddress());
        size += ResponseWriter::println();
        return size;
    }
};

CustomWriter customWriter;

void setup()
{
    // initialize input command reader with main Serial at 9600 bauds, specifying a custom response printer
    input.responseWriter(&customWriter).isSlave().begin(9600, '|', commandDefinitions);
}

void loop()
{
    // your sketch can do here its main tasks
}
