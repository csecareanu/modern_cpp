//

#include <iostream>
#include "class_defines.h"



#define LAVAUES_CPP

#ifdef LAVAUES_CPP
#include "lvalues.cpp"
#endif

int main(int argc, const char * argv[]) {
    
    //lvalues_test_int::run_test();
    
    lvalues_test_class::run_test();
    
    return 0;
}
