#ifndef INPUT_COMMAND_PARAM_H
#define INPUT_COMMAND_PARAM_H

class CommandParam {
  private:
    char* value;
  public:
    CommandParam(char* aValue);
    ~CommandParam();
    char* asString();
    int asInt();
    long asLongInt();
    float asFloat();
};

#endif
