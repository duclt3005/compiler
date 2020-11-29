/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

// loi xay ra
void error(ErrorCode err, int lineNo, int colNo) {
  switch (err) {
  case ERR_ENDOFCOMMENT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_ENDOFCOMMENT); // End of comment expected
    break;
  case ERR_IDENTTOOLONG:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_IDENTTOOLONG);	// Identification too long
    break;
  case ERR_INVALIDCHARCONSTANT:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDCHARCONSTANT); // Invalid const char!
    break;
  case ERR_INVALIDSYMBOL:
    printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDSYMBOL);		// Invalid symbol!
    break;
  case ERR_NUMBERTOOLONG:
	printf("%d-%d:%s\n", lineNo, colNo, ERM_NUMBERTOOLONG);	// Number is too long!
	break;
  case ERR_INVALIDFLOAT:
	printf("%d-%d:%s\n", lineNo, colNo, ERM_INVALIDFLOAT);	// Number is too long!
	break;	
  }
  exit(-1);
}

