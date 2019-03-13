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

//constructor, destructor, ....
#define CLS_CONSTR(CLS) \
CLS() { std::cout << "CALL CONSTRUCTOR: " << STR_CLS_DETAILS(CLS) << "\n"; }

#define CLS_DESTR(CLS) \
~CLS() { std::cout << "CALL DESTRUCTOR: " << STR_CLS_DETAILS(CLS) << "\n"; }

#define CLS_COPY_CONSTR(CLS) \
CLS(const CLS&) { \
std::cout << "CALL COPY CONSTRUCTOR: " << STR_CLS_DETAILS(CLS) << "\n"; }

#define CLS_MOVE_CONSTR(CLS) \
CLS(CLS&&) { \
std::cout << "CALL MOVE CONSTRUCTOR: " << STR_CLS_DETAILS(CLS) << "\n"; }

#define CLS_OP_COPY_ASSIG(CLS) \
CLS& operator = (const CLS&) { \
std::cout << "CALL COPY ASSIG OP : " << STR_CLS_DETAILS(CLS) << "\n"; \
return *this; }

#define CLS_OP_MOVE_ASSIG(CLS) \
CLS& operator = (CLS&&) { \
std::cout << "CALL MOVE ASSIG OP: " << STR_CLS_DETAILS(CLS) << "\n"; \
return *this; }

//deleted members
#define CLS_CONSTR_DEL(CLS)         CLS() = delete;
#define CLS_DESTR_DEL(CLS)          ~CLS() = delete;
#define CLS_COPY_CONSTR_DEL(CLS)    CLS(const CLS&)  = delete;
#define CLS_MOVE_CONSTR_DEL(CLS)    CLS(CLS&&) = delete;
#define CLS_COPY_ASSIG_DEL(CLS)  CLS& operator = (const CLS&) = delete;
#define CLS_MOVE_ASSIG_DEL(CLS)  CLS& operator = (CLS&&) = delete;

//default members
#define CLS_CONSTR_DEF(CLS)         CLS() = default;
#define CLS_DESTR_DEF(CLS)          ~CLS() = default;
#define CLS_COPY_CONSTR_DEF(CLS)    CLS(const CLS&)  = default;
#define CLS_MOVE_CONSTR_DEF(CLS)    CLS(CLS&&) = default;
#define CLS_COPY_ASSIG_DEF(CLS)  CLS& operator = (const CLS&) = default;
#define CLS_MOVE_ASSIG_DEF(CLS)  CLS& operator = (CLS&&) = default;


#endif /* class_defines_h */
