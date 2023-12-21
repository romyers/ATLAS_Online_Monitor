#include "UIException.h"

using namespace std;



UIException::UIException(const std::string &msg) : message(msg) {}

const char *UIException::what() {

    return message.data();

}
