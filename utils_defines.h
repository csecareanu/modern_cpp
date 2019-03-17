//
//  utils_defines.h
//

#ifndef utils_defines_h
#define utils_defines_h

#define STR(str) #str

/*
 Daca am #define CLS myClass atunci #str va returna "CLS"
 Atuci aplelez STR(myClass) care va intoarce "myClass"
 */
#define STR2(str) STR(str)

#define EMPTY_LINE static int __t##_LINE_ = 0; __t##_LINE_++;

#endif /* utils_defines_h */
