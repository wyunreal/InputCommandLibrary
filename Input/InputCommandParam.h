#ifndef INPUT_COMMAND_PARAM_H
#define INPUT_COMMAND_PARAM_H

class InputCommandParam {
  private:
    char* value;
  public:
    InputCommandParam(char* aValue);
    ~InputCommandParam();
    char* asString();
    int asInt();
    float asFloat();
};

#endif
