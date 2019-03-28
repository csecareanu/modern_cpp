//
//  class_defines.h
//

#ifndef class_defines_h
#define class_defines_h

#include "utils_defines.h"

/*
 Daca am destructor atunci nu se creeaza
    move constructor(5) sau move assignment operator(6)
 
 -----------------------------------
 1. constr             | | | | | | |
 -----------------------------------
 2. destr              | | | | |5|6|
 -----------------------------------
 3. copy constr        | | | | |5|6|
 -----------------------------------
 4. copy assig op      | | | | |5|6|
 -----------------------------------
 5. move constr        | | |3|4| |6|
 -----------------------------------
 6. move assig op      | | |3|4|5| |
 -----------------------------------
 */

//utils
#define STR_CLS_DETAILS(CLS) STR(CLS) << "_" << this

#define CLS_METHOD_INFO(METHOD_NAME) \
std::cout << "CALL " << METHOD_NAME << ": " << STR_CLS_DETAILS(CLS) << "\n";


//constructor, destructor, ....
#define CLS_CONSTR \
CLS() noexcept { CLS_METHOD_INFO("CONSTRUCTOR"); }

#define CLS_DESTR \
~CLS() noexcept { CLS_METHOD_INFO("DESTRUCTOR"); }

#define CLS_COPY_CONSTR \
CLS(const CLS&) noexcept { CLS_METHOD_INFO("COPY CONSTRUCTOR"); }

#define CLS_MOVE_CONSTR \
CLS(CLS&&) noexcept { CLS_METHOD_INFO("MOVE CONSTRUCTOR"); }

#define CLS_COPY_ASSIG \
CLS& operator = (const CLS&) noexcept { CLS_METHOD_INFO("COPY ASSIG OP"); return *this; }

#define CLS_MOVE_ASSIG \
CLS& operator = (CLS&&) noexcept { CLS_METHOD_INFO("MOVE ASSIG OP"); return *this; }

//deleted members
#define CLS_CONSTR_DEL        CLS() = delete;
#define CLS_DESTR_DEL         ~CLS() = delete;
#define CLS_COPY_CONSTR_DEL   CLS(const CLS&)  = delete;
#define CLS_MOVE_CONSTR_DEL   CLS(CLS&&) = delete;
#define CLS_COPY_ASSIG_DEL    CLS& operator = (const CLS&) = delete;
#define CLS_MOVE_ASSIG_DEL    CLS& operator = (CLS&&) = delete;

//default members
#define CLS_CONSTR_DEF        CLS() = default;
#define CLS_DESTR_DEF         ~CLS() = default;
#define CLS_COPY_CONSTR_DEF   CLS(const CLS&)  = default;
#define CLS_MOVE_CONSTR_DEF   CLS(CLS&&) = default;
#define CLS_COPY_ASSIG_DEF    CLS& operator = (const CLS&) = default;
#define CLS_MOVE_ASSIG_DEF    CLS& operator = (CLS&&) = default;


#endif /* class_defines_h */
