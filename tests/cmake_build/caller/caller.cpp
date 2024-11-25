#include <kwargscpp/kwargs.h>
#include <iostream>

int main() {
    kwargscpp::DictType args;
    args["key"] = "value";
    std::cout<< kwargscpp::to_string(args)<<std::endl;
    return 0;
}
