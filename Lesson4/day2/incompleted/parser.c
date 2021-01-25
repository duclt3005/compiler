#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "semantics.h"
#include "error.h"
#include "debug.h"

Token *currentToken;
Token *lookAhead;
int check = 0;
extern Type *intType;
extern Type *charType;
extern SymTab *symtab;
int checkFor=0;
int checkFunction = 0;
int checkProcedure=0;
int indexArr = 0;
int checkMod1= 0;
int checkMod2=0;
void scan(void)
{
  Token *tmp = currentToken;
  currentToken = lookAhead;
  lookAhead = getValidToken();
  free(tmp);
}

void eat(TokenType tokenType)
{
  if (lookAhead->tokenType == tokenType)
  {
    printToken(lookAhead);
    scan();
  }
  else
  {
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileProgram(void)
{
  // create, enter, and exit program block
  Object *obj;
  eat(KW_PROGRAM);
  eat(TK_IDENT);
  obj = createProgramObject(currentToken->string);

  eat(SB_SEMICOLON);
  enterBlock(obj->progAttrs->scope);
  compileBlock();
  exitBlock();
  eat(SB_PERIOD);
}

void compileBlock(void)
{
  // create and declare constant objects
  Object *obj = NULL;
  if (lookAhead->tokenType == KW_CONST)
  {
    eat(KW_CONST);

    do
    {
      eat(TK_IDENT);
      // Check if a constant identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      obj = createConstantObject(currentToken->string);

      eat(SB_EQ);
      obj->constAttrs->value = compileConstant();
      eat(SB_SEMICOLON);
      declareObject(obj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock2();
  }
  else
    compileBlock2();
}

void compileBlock2(void)
{
  // create and declare type objects
  Object *obj = NULL;
  if (lookAhead->tokenType == KW_TYPE)
  {
    eat(KW_TYPE);

    do
    {
      eat(TK_IDENT);
      // Check if a type identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      obj = createTypeObject(currentToken->string);

      eat(SB_EQ);
      obj->typeAttrs->actualType = compileType();
      eat(SB_SEMICOLON);

      declareObject(obj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock3();
  }
  else
    compileBlock3();
}

void compileBlock3(void)
{
  // create and declare variable objects
  Object *obj = NULL;
  if (lookAhead->tokenType == KW_VAR)
  {
    eat(KW_VAR);

    do
    {
      eat(TK_IDENT);
      // Check if a variable identifier is fresh in the block
      checkFreshIdent(currentToken->string);
      obj = createVariableObject(currentToken->string);

      eat(SB_COLON);
      obj->varAttrs->type = compileType();

      eat(SB_SEMICOLON);
      declareObject(obj);
    } while (lookAhead->tokenType == TK_IDENT);

    compileBlock4();
  }
  else
    compileBlock4();
}

void compileBlock4(void)
{
  compileSubDecls();
  compileBlock5();
}

void compileBlock5(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileSubDecls(void)
{
  while ((lookAhead->tokenType == KW_FUNCTION) || (lookAhead->tokenType == KW_PROCEDURE))
  {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();
    else
      compileProcDecl();
  }
}

void compileFuncDecl(void)
{
  Object* funcObj;
  Type* returnType;

  eat(KW_FUNCTION);
  checkFunction=1;
  eat(TK_IDENT);
  // Check if a function identifier is fresh in the block
  checkFreshIdent(currentToken->string);

  // create the function object
  funcObj = createFunctionObject(currentToken->string);
  // declare the function object
  declareObject(funcObj);
  // enter the function's block
  enterBlock(funcObj->funcAttrs->scope);
  // parse the function's parameters
  compileParams();
  eat(SB_COLON);
  // get the funtion's return type
  returnType = compileBasicType();
  funcObj->funcAttrs->returnType = returnType;

  eat(SB_SEMICOLON);
  compileBlock();
  if(checkFunction<2){
    printf("There is at least one assignment LValue same Function ID\n");
  }
  eat(SB_SEMICOLON);
  // exit the function block
  exitBlock();

}

void compileProcDecl(void)
{
  // create and declare a procedure object
  Object *obj = NULL;

  eat(KW_PROCEDURE);
  eat(TK_IDENT);
  // Check if a procedure identifier is fresh in the block
  checkFreshIdent(currentToken->string);

  obj = createProcedureObject(currentToken->string);
  declareObject(obj);
  enterBlock(obj->procAttrs->scope);
  compileParams();
  eat(SB_SEMICOLON);
  compileBlock();
  eat(SB_SEMICOLON);
  exitBlock();

}

ConstantValue *compileUnsignedConstant(void)
{
  // create and return an unsigned constant value
  ConstantValue *constValue = NULL;
  Object *obj;
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value2);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // obj = lookupObject(currentToken->string);

    // if (obj == NULL)
    // {
    //   error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    // }
    // else if (obj->kind != OBJ_CONSTANT)
    // {
    //   error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);
    // }
    obj = checkDeclaredConstant(currentToken->string);
    if (obj != NULL)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else
      error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

    // Copy constant value from declared constant
    //constValue = (ConstantValue *)malloc(sizeof(ConstantValue));
    //*constValue = *(obj->constAttrs->value);
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  return constValue;
}

ConstantValue *compileConstant(void)
{
  // create and return a constant
  ConstantValue *constValue = NULL;

  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    constValue = compileConstant2();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    constValue = compileConstant2();
    if (constValue->type == TP_INT)
    {
      constValue->intValue = -constValue->intValue;
    }
    if (constValue->type == TP_FLOAT)
    {
      constValue->floatValue = -constValue->floatValue;
    }
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    constValue = makeCharConstant(currentToken->string[0]);
    break;
  default:
    constValue = compileConstant2();
    break;
  }
  return constValue;
}

ConstantValue *compileConstant2(void)
{
  // create and return a constant value
  ConstantValue *constValue = NULL;
  Object *obj;
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    constValue = makeIntConstant(currentToken->value);
    break;
  case TK_FLOAT:
    eat(TK_FLOAT);
    constValue = makeFloatConstant(currentToken->value2);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    // check if the integer constant identifier is declared and get its value
    obj = checkDeclaredConstant(currentToken->string);
    //obj = lookupObject(currentToken->string);

    // if (obj == NULL)
    //   error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    // else if (obj->kind != OBJ_CONSTANT)
    //   error(ERR_INVALID_CONSTANT, currentToken->lineNo, currentToken->colNo);

    // // Copy constant value from declared constant
    // constValue = duplicateConstantValue(obj->constAttrs->value);
    if (obj != NULL)
      constValue = duplicateConstantValue(obj->constAttrs->value);
    else
      error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);
    break;
  default:
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  return constValue;
}

Type *compileType(void)
{
  // create and return a type
  Type *type;
  Type *elementType;
  int arraySize;
  Object *obj;
  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    // type = (Type *)malloc(sizeof(Type));
    // type->typeClass = TP_INT;
    type = makeIntType();
    break;

  case KW_FLOAT:
    eat(KW_FLOAT);
    // type = (Type *)malloc(sizeof(Type));
    // type->typeClass = TP_FLOAT;
    type = makeFloatType();
    break;

  case KW_CHAR:
    eat(KW_CHAR);
    // type = (Type *)malloc(sizeof(Type));
    // type->typeClass = TP_CHAR;
    type = makeCharType();
    break;

  case KW_ARRAY:
    // type = (Type *)malloc(sizeof(Type));
    // type->typeClass = TP_ARRAY;
    // eat(KW_ARRAY);
    // eat(SB_LSEL);
    // eat(TK_NUMBER);
    // type->arraySize = currentToken->value;
    // eat(SB_RSEL);
    // eat(KW_OF);
    // type->elementType = compileType();
    // break;
    eat(KW_ARRAY);
    eat(SB_LSEL);
    eat(TK_NUMBER);

    arraySize = currentToken->value;

    eat(SB_RSEL);
    eat(KW_OF);
    elementType = compileType();
    type = makeArrayType(arraySize, elementType);
    break;
  case TK_IDENT:
    // eat(TK_IDENT);
    // Object *obj = lookupObject(currentToken->string);

    // if (obj == NULL)
    // {
    //   error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);
    // }
    // else if (obj->kind != OBJ_TYPE)
    // {
    //   error(ERR_INVALID_TYPE, currentToken->lineNo, currentToken->colNo);
    // }

    // // Copy type from declared one
    // type = duplicateType(obj->typeAttrs->actualType);
    // break;
    eat(TK_IDENT);
    // check if the type identifier is declared and get its actual type
    obj = checkDeclaredType(currentToken->string);
    if (obj != NULL)
      type = duplicateType(obj->typeAttrs->actualType);
    else
      error(ERR_UNDECLARED_TYPE, currentToken->colNo, currentToken->lineNo);

    break;
  default:
    error(ERR_INVALID_TYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  return type;
}

Type *compileBasicType(void)
{
  // create and return a basic type
  Type *type = NULL;

  switch (lookAhead->tokenType)
  {
  case KW_INTEGER:
    eat(KW_INTEGER);
    type = (Type *)malloc(sizeof(Type));
    type->typeClass = TP_INT;
    break;
  case KW_FLOAT:
    eat(KW_FLOAT);
    type = (Type *)malloc(sizeof(Type));
    type->typeClass = TP_FLOAT;
    break;
  case KW_CHAR:
    eat(KW_CHAR);
    type = (Type *)malloc(sizeof(Type));
    type->typeClass = TP_CHAR;
    break;
  default:
    error(ERR_INVALID_BASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  return type;
}

void compileParams(void)
{
  if (lookAhead->tokenType == SB_LPAR)
  {
    eat(SB_LPAR);
    compileParam();
    while (lookAhead->tokenType == SB_SEMICOLON)
    {
      eat(SB_SEMICOLON);
      compileParam();
    }
    eat(SB_RPAR);
  }
  // else{
  //   Object *param= createParameterObject(currentToken->string,0, symtab->currentScope->owner);
  //   declareObject(param);
  //   checkProcedure=1;
  // }
}

void compileParam(void)
{
  // create and declare a parameter
  Object *param;
  Type *type;
  enum ParamKind paramKind;

  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    paramKind = PARAM_VALUE;
    break;
  case KW_VAR:
    eat(KW_VAR);
    paramKind = PARAM_REFERENCE;
    break;
  default:
    error(ERR_INVALID_PARAMETER, lookAhead->lineNo, lookAhead->colNo);
    break;
  }

  eat(TK_IDENT);
  // check if the parameter identifier is fresh in the block
  checkFreshIdent(currentToken->string);

  param = createParameterObject(currentToken->string, paramKind, symtab->currentScope->owner);
  eat(SB_COLON);
  type = compileBasicType();
  param->paramAttrs->type = type;
  declareObject(param);
}

void compileStatements(void)
{
  compileStatement();
  while (lookAhead->tokenType == SB_SEMICOLON)
  {
    eat(SB_SEMICOLON);
    compileStatement();
  }
}

void compileStatement(void)
{
  switch (lookAhead->tokenType)
  {
  case TK_IDENT:
    checkDeclaredLValueIdent1(lookAhead->string, &checkFunction);
    compileAssignSt();
    break;
  case KW_CALL:
    compileCallSt();
    break;
  case KW_BEGIN:
    compileGroupSt();
    break;
  case KW_IF:
    compileIfSt();
    break;
  case KW_WHILE:
    compileWhileSt();
    break;
  case KW_FOR:
    compileForSt();
    break;
    // EmptySt needs to check FOLLOW tokens
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
    break;
    // Error occurs
  default:
    error(ERR_INVALID_STATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}


Type* compileLValue(void) {
  // parse a lvalue (a variable, an array element, a parameter, the current function identifier)
  Object* var = NULL;
  Type* varType = NULL;

  eat(TK_IDENT);
  // check if the identifier is a function identifier, or a variable identifier, or a parameter  
  var = checkDeclaredLValueIdent(currentToken->string);
  if (var->kind == OBJ_VARIABLE)
    varType = compileIndexes(var->varAttrs->type);
  else if (var->kind == OBJ_FUNCTION)
    varType = var->funcAttrs->returnType;
  else if (var->kind == OBJ_PARAMETER)
    varType = var->paramAttrs->type;

  return varType;
}


void compileAssignSt(void)
{
  Type *lvalueType = NULL;
  Type *expType = NULL;

  lvalueType = compileLValue();
 // printf("-----%d\n", lvalueType->typeClass);
  switch (lookAhead->tokenType)
  {
  case SB_ASSIGN_PLUS:
    eat(SB_ASSIGN_PLUS);
    break;
  case SB_ASSIGN_MINUS:
    eat(SB_ASSIGN_MINUS);
    break;
  case SB_ASSIGN_TIME:
    eat(SB_ASSIGN_TIME);
    break;
  case SB_ASSIGN_SLASH:
    eat(SB_ASSIGN_SLASH);
    break;
  case SB_SHIFT_LEFT:
    eat(SB_SHIFT_LEFT);
    break;
  case SB_SHIFT_RIGHT:
    eat(SB_SHIFT_RIGHT);
    break;
  case SB_ASSIGN:
    eat(SB_ASSIGN);
    break;
  default:
    printf("%d-%d:Missing assign\n", lookAhead->lineNo, lookAhead->colNo);
    exit(0);
  }
  //eat(SB_ASSIGN);
  expType = compileExpression();
 // printf(">>>>>>>>>>>>%d\n", lvalueType->typeClass);
  //compileExpression();
   checkTypeEquality(lvalueType, expType);
}

void compileCallSt(void) {
  Object* proc;

  eat(KW_CALL);
  eat(TK_IDENT);

  proc = checkDeclaredProcedure(currentToken->string);

  compileArguments(proc->procAttrs->paramList);
}

void compileGroupSt(void)
{
  eat(KW_BEGIN);
  compileStatements();
  eat(KW_END);
}

void compileIfSt(void)
{
  eat(KW_IF);
  compileCondition();
  eat(KW_THEN);
  compileStatement();
  if (lookAhead->tokenType == KW_ELSE)
    compileElseSt();
}

void compileElseSt(void)
{
  eat(KW_ELSE);
  compileStatement();
}

void compileWhileSt(void)
{
  eat(KW_WHILE);
  compileCondition();
  eat(KW_DO);
  compileStatement();
}

void compileForSt(void)
{
  eat(KW_FOR);
  eat(TK_IDENT);
  Object* var =checkDeclaredVariable(currentToken->string);
  if (var== NULL){
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);
  }
   if(var->varAttrs->type->typeClass != TP_INT){
      error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
   }
  eat(SB_ASSIGN);
  checkFor=1;
  compileExpression();
  eat(KW_TO);
  compileExpression();
  eat(KW_DO);
  checkFor=0;
  compileStatement();
}


void compileArgument(Object* param) {
  // parse an argument, and check type consistency
  //       If the corresponding parameter is a reference, the argument must be a lvalue
  if (param->paramAttrs->kind == PARAM_REFERENCE) {
    if (lookAhead->tokenType == TK_IDENT) {
        checkDeclaredLValueIdent(lookAhead->string);
    } else {
        error(ERR_TYPE_INCONSISTENCY, lookAhead->lineNo, lookAhead->colNo);
    }
  }

  Type *argType = compileExpression();
  checkTypeEquality(argType, param->paramAttrs->type);

}

void compileArguments(ObjectNode* paramList) {
  // parse a list of arguments, check the consistency of the arguments and the given parameters
  switch (lookAhead->tokenType) {
  case SB_LPAR:
    eat(SB_LPAR);
  //   if((checkProcedure!=0) && (paramList->object->kind == OBJ_PROCEDURE)){
  //     printf("*******************\n");
  //     error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
  // }
    compileArgument(paramList->object);

    while (lookAhead->tokenType == SB_COMMA) {
      eat(SB_COMMA);
      paramList = paramList->next;
      if (paramList != NULL)
        compileArgument(paramList->object);
      else
        error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    }
    // param list still has next one when we've done parsing arguments
    // means number of arguments doesn't match number of params
    if (paramList->next != NULL)
      error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);

    eat(SB_RPAR);
    break;
    // Check FOLLOW set
  case SB_TIMES:
  case SB_SLASH:
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
      //break;
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    // Param list exists but we don't see left parenthesis
    // if (paramList->object != NULL){
    //       error(ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    // }
    break;
  default:
    error(ERR_INVALID_ARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
  }
}

void compileCondition(void)
{
  compileExpression();
  switch (lookAhead->tokenType)
  {
  case SB_EQ:
    eat(SB_EQ);
    break;
  case SB_NEQ:
    eat(SB_NEQ);
    break;
  case SB_LE:
    eat(SB_LE);
    break;
  case SB_LT:
    eat(SB_LT);
    break;
  case SB_GE:
    eat(SB_GE);
    break;
  case SB_GT:
    eat(SB_GT);
    break;
  default:
    error(ERR_INVALID_COMPARATOR, lookAhead->lineNo, lookAhead->colNo);
  }

  compileExpression();
}

Type* compileExpression(void)
{
  Type* type;
  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    //compileExpression2();
    type = compileExpression2();
    //checkIntType(type);
    checkFloatType(type);
    
    break;
  case SB_MINUS:
    eat(SB_MINUS);
     type = compileExpression2();
    //checkIntType(type);
    checkFloatType(type);
    //compileExpression2();
    break;
  default:
    type = compileExpression2();
  }
  return type;
}

Type* compileExpression2(void)
{
  Type* type;

  type = compileTerm();
  compileExpression3();

  return type;
}

void compileExpression3(void)
{
   Type* type;

  switch (lookAhead->tokenType)
  {
  case SB_PLUS:
    eat(SB_PLUS);
    type = compileTerm();
   //checkIntType(type);
   checkFloatType(type);
    compileExpression3();
    break;
  case SB_MINUS:
    eat(SB_MINUS);
    type = compileTerm();
    //checkIntType(type);
    checkFloatType(type);
    compileExpression3();
    break;
    // check the FOLLOW set
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_EXPRESSION, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileTerm(void)
{
  Type* type;

  type = compileFactor();
  compileTerm2();

  return type;
}

void compileTerm2(void)
{
  Type* type;
  switch (lookAhead->tokenType)
  {
  case SB_TIMES:
    eat(SB_TIMES);
    type = compileFactor();
    checkFloatType(type);
    compileTerm2();
    break;
  case SB_SLASH:
    eat(SB_SLASH);
     type = compileFactor();
    checkFloatType(type);
    compileTerm2();
    break;
  case SB_MOD:
    //printf("====================%s\n", currentToken->string);
    //checkIntType(type);
    eat(SB_MOD);
      //printf("+++++++%d++++++\n", checkMod1);

    //compileFactor();
    type = compileFactor();
    checkIntType(type);
    // checkMod2=1;
    // if( (checkMod1 ==1) && (checkMod2 ==1)){
    //     error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
    // }
    compileTerm2();
    break;
    // check the FOLLOW set
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
  default:
    error(ERR_INVALID_TERM, lookAhead->lineNo, lookAhead->colNo);
  }
}

Type* compileFactor(void)
{
  Object* obj = NULL;
  Type* type = NULL;
  switch (lookAhead->tokenType)
  {
  case TK_NUMBER:
    eat(TK_NUMBER);
    type = makeIntType();
  // printf("!~~~~%d~~%s~~\n", currentToken->value, currentToken->string);
    indexArr= currentToken->value;
    break;
  case TK_FLOAT:
    if(checkFor==1){
      error(ERR_FOR_INDEX_FLOAT, lookAhead->lineNo, lookAhead->colNo);
    }
    // checkMod1 = 1;
    // printf("===========%d\n", lookAhead->tokenType);
    eat(TK_FLOAT);
    type = makeFloatType();
    break;
  case TK_CHAR:
    eat(TK_CHAR);
    type = makeCharType();
    break;
  case SB_MOD:
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  case TK_IDENT:
    eat(TK_IDENT);
    
    // check if the identifier is declared
    obj = checkDeclaredIdent(currentToken->string);
    // switch (lookAhead->tokenType)
    // {
    // case SB_LPAR:
    //   compileArguments();
    //   break;
    // case SB_LSEL:
    //   compileIndexes();
    //   break;
    // default:
    //   break;
    // }
    switch (obj->kind)
    {
    case OBJ_CONSTANT:
      type = makeIntType();
      type->typeClass = obj->constAttrs->value->type;
      break;
    case OBJ_VARIABLE:
        if (obj->varAttrs->type->typeClass != TP_ARRAY){
            if((obj->varAttrs->type->typeClass != TP_INT) && (checkFor==1)){
              error(ERR_FOR_INDEX_FLOAT, currentToken->lineNo, currentToken->colNo);
            }
          type = obj->varAttrs->type;
        }
        else{
            type = compileIndexes(obj->varAttrs->type);
            //printf("--->>>>>>>>>>>>>%d\n", type->typeClass);
        }
      break;
    case OBJ_PARAMETER:
      type = obj->paramAttrs->type;
      break;
    case OBJ_FUNCTION:
       type = obj->funcAttrs->returnType;
      compileArguments(obj->funcAttrs->paramList);
      break;
    default:
      error(ERR_INVALID_FACTOR, currentToken->lineNo, currentToken->colNo);
      break;
    }
    break;
  default:
    error(ERR_INVALID_FACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
   //printf("-----_______++++++\n");
  return type;
}

Type* compileIndexes(Type* arrayType) {
  // parse a sequence of indexes, check the consistency to the arrayType, and return the element type
  Type *idxType = NULL;
    Type *elmType = NULL;
  int size = arrayType->arraySize;
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);

    // if current element is not of array type,
    // then the access to the next dimension is invalid
    checkArrayType(arrayType);

    idxType = compileExpression();
    if(indexArr<=0 || indexArr>size){
       error(ERR_ARRAY_OVERFLOW, lookAhead->lineNo, lookAhead->colNo);
    }
    checkIntType(idxType);

    eat(SB_RSEL);
    // Down 1 level of dimension
    arrayType = arrayType->elementType;
  }

  // arrayType becomes elmType when we traverse to the last dimension
  elmType = arrayType;
  //printf("=======++++++++========\n");
  return elmType;
}

int compile(char *fileName)
{
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  currentToken = NULL;
  lookAhead = getValidToken();

  initSymTab();

  compileProgram();
printObjectList(symtab->globalObjectList, 0);
  printObject(symtab->program, 0);
  //printObjectList(symtab->globalObjectList, 0);
  cleanSymTab();

  free(currentToken);
  free(lookAhead);
  closeInputStream();
  return IO_SUCCESS;
}