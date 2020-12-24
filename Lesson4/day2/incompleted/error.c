/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define NUM_OF_ERRORS 29

struct ErrorMessage
{
    ErrorCode errorCode;
    char *message;
};

struct ErrorMessage errors[30] = {
    {ERR_END_OF_COMMENT, "End of comment expected."},                   // không đóng ngoặc comment ở cuối
    {ERR_IDENT_TOO_LONG, "Identifier too long."},                       // tên IDENT quá dài
    {ERR_INVALID_CONSTANT_CHAR, "Invalid char constant."},              // const m = ' abc  ; 
    {ERR_INVALID_SYMBOL, "Invalid symbol."},                            // $, &
    {ERR_INVALID_IDENT, "An identifier expected."},
    {ERR_INVALID_CONSTANT, "A constant expected."},
    {ERR_INVALID_TYPE, "A type expected."},                             // CONst  d = 123.4 ; d= -d; Type t = Float ;Var n : d;
    {ERR_INVALID_BASICTYPE, "A basic type expected."},                  //F(n : Integersddsdv) 
    {ERR_INVALID_VARIABLE, "A variable expected."},
    {ERR_INVALID_FUNCTION, "A function identifier expected."},
    {ERR_INVALID_PROCEDURE, "A procedure identifier expected."},
    {ERR_INVALID_PARAMETER, "A parameter expected."},                   //Function F(FLoat n : Integer) : Integer;
    {ERR_INVALID_STATEMENT, "Invalid statement."},                       
    {ERR_INVALID_COMPARATOR, "A comparator expected."},
    {ERR_INVALID_EXPRESSION, "Invalid expression."},                    
    {ERR_INVALID_TERM, "Invalid term."},                                // for n:=3 begin
    {ERR_INVALID_FACTOR, "Invalid factor."},                            // for n: = )3
    {ERR_INVALID_LVALUE, "Invalid lvalue in assignment."},
    {ERR_INVALID_ARGUMENTS, "Wrong arguments."},
    {ERR_UNDECLARED_IDENT, "Undeclared identifier."},                   // trong function:  Function F(n : Integer) : Integer;  
                                                                        //                      If m = 0 Then F := 1 Else F := N * F (N - 1);
    {ERR_UNDECLARED_CONSTANT, "Undeclared constant."},                  // CONst  m = d 123;      
    {ERR_UNDECLARED_INT_CONSTANT, "Undeclared integer constant."},
    {ERR_UNDECLARED_TYPE, "Undeclared type."},                          // var t : akdss 
    {ERR_UNDECLARED_VARIABLE, "Undeclared variable."},                  
    {ERR_UNDECLARED_FUNCTION, "Undeclared function."},                  // call abc(c1);   --> chưa khời tạo function()  
    {ERR_UNDECLARED_PROCEDURE, "Undeclared procedure."},                // call abc(c1);   --> chưa khời tạo procedure()
    {ERR_DUPLICATE_IDENT, "Duplicate identifier."},                     // Const c1 = 10; c2 = 'a'; c2 = 10;
    {ERR_TYPE_INCONSISTENCY, "Type inconsistency"},
    {ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, "The number of arguments and the number of parameters are inconsistent."},
    {ERR_INVALIDFACTOR ,"Invalid factor!"}};

void error(ErrorCode err, int lineNo, int colNo)
{
    int i;
    for (i = 0; i < NUM_OF_ERRORS; i++)
        if (errors[i].errorCode == err)
        {
            printf("%d-%d:%s\n", lineNo, colNo, errors[i].message);
            exit(0);
        }
}

void missingToken(TokenType tokenType, int lineNo, int colNo)
{
    printf("%d-%d:Missing %s\n", lineNo, colNo, tokenToString(tokenType));
    exit(0);
}

void assert(char *msg)
{
    printf("%s\n", msg);
}
