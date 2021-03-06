/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (currentChar != -1 && charCodes[currentChar] == CHAR_SPACE)
	readChar();
}

void skipComment() {
  while (1) {
    readChar();

    if (currentChar == -1) { 
      error(ERR_ENDOFCOMMENT, lineNo, colNo); // Loi comment khong dong
    } 
	else if (charCodes[currentChar] == CHAR_TIMES) { // ki tu *
      readChar();

      if (currentChar == -1) { 
        error(ERR_ENDOFCOMMENT, lineNo, colNo);  // Loi comment khong dong
      } 
	  else if (charCodes[currentChar] == CHAR_RPAR) { // ki tu ) -> ket thuc comment
        readChar();
        return;
      }
    }
  }
}

// ============= check identifier token =======================
Token* readIdentKeyword(void) {
  int count = 0;
  Token* token = makeToken(TK_IDENT, lineNo, colNo);		// token Identification

  while (charCodes[currentChar] == CHAR_LETTER ||charCodes[currentChar] == CHAR_DIGIT ) {   // ki tu hien tai la char
    token->string[count] = currentChar; 		// them vao token
    count++;									// ki tu tiep theo trong token
    readChar();
  }
  token->string[count] = '\0';		// ket thuc token

  if (count > MAX_IDENT_LEN) {
    error(ERR_IDENTTOOLONG, lineNo, colNo - count);		// ident qua dai
  } 
  else {
    TokenType type = checkKeyword(token->string);		// keyword gi
    if (type != TK_NONE) {
      token->tokenType = type;							// khong phai KW
    }
  }

  return token;
}

// ============= check number token =======================
Token* readNumber(void) {
  int count = 0;
  Token* token = makeToken(TK_NUMBER, lineNo, colNo);		// token Number
	token->string[count] = currentChar;						// them vao token
    count++;												// ki tu tiep theo trong token
    readChar();
    if(charCodes[currentChar]== CHAR_PERIOD){
    	
    	token->string[count] = currentChar;						// them vao token
    	count++;
		readChar();
//		if(charCodes[currentChar] == CHAR_PERIOD){
//			error(ERR_INVALIDFLOAT, token->lineNo, token->colNo);
//		}
		while (charCodes[currentChar] == CHAR_DIGIT) {
			token->string[count] = currentChar;						// them vao token
			count++;												// ki tu tiep theo trong token
			readChar();
			
		}	
		token->tokenType= TK_FLOAT;
		token->string[count] = '\0';								// ket thuc token
  		token->value = atof(token->string);
	}
    else{
	  while (charCodes[currentChar] == CHAR_DIGIT) {			// ki tu hien tai la digit
	//	if (count > 9) {
	//		error(ERR_NUMBERTOOLONG, token->lineNo, token->colNo);	// so >9 chu so ->error
	//	}
	    token->string[count] = currentChar;						// them vao token
	    count++;												// ki tu tiep theo trong token
	    readChar();
	  }
	
	  token->string[count] = '\0';								// ket thuc token
	  token->value = atoi(token->string);						// chuyen ve Integer
	}
  return token;
}

// ============= check const token =======================
Token* readConstChar(void) {
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  readChar();
  if (currentChar == -1) {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);	// loi sai CONST
  } 
  else {
	switch(charCodes[currentChar]) {
	case CHAR_SINGLEQUOTE:				// �
		readChar();
		if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
		    token->string[0] = currentChar;

		    readChar();
		    if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
		        token->string[1] = '\0';

		        readChar();
		        return token;
		    } else {
		        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);	// loi sai CONST
		    }

		} else {
			error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);		// loi sai CONST
		}
		break;
	default:
        token->string[0] = currentChar;
		readChar();

		switch(charCodes[currentChar]) {
		case CHAR_SINGLEQUOTE:
			token->string[1] = '\0';

			readChar();
			return token;
		default:
			error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo); 	// loi sai CONST
			break;
		}
		break;
	}

  }
  return token;
}

// ============= get token form source code =======================
Token* getToken(void) {
  	Token *token;

  	if (currentChar == EOF)
    	return makeToken(TK_EOF, lineNo, colNo);

  	switch (charCodes[currentChar]) {
  	case CHAR_LETTER: return readIdentKeyword();
  	
  	case CHAR_DIGIT: return readNumber();
  	
  	case CHAR_SPACE: skipBlank(); 
  					return getToken();
  					
	case CHAR_LPAR:											// (
	    token = makeToken(TK_NONE, lineNo, colNo);
	    readChar();
	
	    switch(charCodes[currentChar]) {
	    case CHAR_PERIOD:									// .
	      token->tokenType = SB_LSEL;
	      readChar();
	      return token;
	    case CHAR_TIMES:									// *
	      free(token);
	      skipComment();
	      return getToken();
	    case CHAR_SPACE:									// SPACE	
	      readChar();
	      return getToken();
	    default:
	      token->tokenType = SB_LPAR;
	      return token;
	    }
	case CHAR_SINGLEQUOTE:									// '
    	return readConstChar();
    	
    case CHAR_LT:											// <
    	token = makeToken(TK_NONE, lineNo, colNo);
    	readChar();
    	switch(charCodes[currentChar]) {
    		case CHAR_EQ:									// =
      			token->tokenType = SB_LE;					// <=
      			readChar();
      			return token;
    	default:
				token->tokenType = SB_LT;					// <
				return token;
    	}
    
    case CHAR_GT:											// >
    	token = makeToken(SB_GT, lineNo, colNo);
    	readChar();
    	if (charCodes[currentChar] == CHAR_EQ) {			// =
      		token->tokenType = SB_GE; 						// >=
      		readChar();
    	}
    	return token;
    
	case CHAR_EXCLAIMATION:									// !
    	token = makeToken(TK_NONE, lineNo, colNo);
    	readChar();
	    if (charCodes[currentChar] != CHAR_EQ) {			// =
	      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);		// sai ki hieu  ==
	    } else {
	      token->tokenType = SB_NEQ;						// !=
	    }
	    return token;	
	
	case CHAR_PERIOD:										// .
	    token = makeToken(SB_PERIOD, lineNo, colNo);
	    readChar();
	    if (charCodes[currentChar] == CHAR_RPAR) {			// )
	      token->tokenType = SB_RSEL;
	      readChar();
	    }
	    return token;
	
	case CHAR_COLON:										// :
	    token = makeToken(SB_SEMICOLON, lineNo, colNo);
	    readChar();
	    if (charCodes[currentChar] == CHAR_EQ) {			// =
	      token->tokenType = SB_ASSIGN;						// :=
	      readChar();
	    }
	    return token;	  
		 
  	case CHAR_PLUS:											// +
    	token = makeToken(SB_PLUS, lineNo, colNo);
    	readChar();
    	return token;
    	
  	case CHAR_MINUS:										// -
    	token = makeToken(SB_MINUS, lineNo, colNo);
    	readChar();
    	return token;
    	
	case CHAR_TIMES:										// *
	    token = makeToken(SB_TIMES, lineNo, colNo);
	    readChar();
	    return token;
	    
  	case CHAR_SLASH:										// /
    	token = makeToken(SB_SLASH, lineNo, colNo);
    	readChar();
    	return token;
    	
  	case CHAR_EQ:											// =
    	token = makeToken(SB_EQ, lineNo, colNo);
    	readChar();
    	return token;
    
  	case CHAR_COMMA:										// ,
    	token = makeToken(SB_COMMA, lineNo, colNo);
    	readChar();
    	return token;
  	case CHAR_SEMICOLON:									// ;
    	token = makeToken(SB_SEMICOLON, lineNo, colNo);
    	readChar();
    	return token;
  	case CHAR_RPAR:											// )
    	token = makeToken(SB_RPAR, lineNo, colNo);
    	readChar();
    	return token;
    case CHAR_PERCENT:											// %
    	token = makeToken(SB_PERCENT, lineNo, colNo);
    	readChar();
    	return token;	
  	default:
	    token = makeToken(TK_NONE, lineNo, colNo);
	    error(ERR_INVALIDSYMBOL, lineNo, colNo);
	    readChar();
	    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_FLOAT: printf("KW_FLOAT\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_PERCENT: printf("SB_PERCENT\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}



