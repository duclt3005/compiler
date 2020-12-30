/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

static int on_search = 0;

Object* lookupObject(char *name) {
  Scope* scope = symtab->currentScope;
  Object* obj;

  while (scope != NULL) {
    obj = findObject(scope->objList, name);
    if (obj != NULL) return obj;
    scope = scope->outer;
  }
  obj = findObject(symtab->globalObjectList, name);
  if (obj != NULL) return obj;
  return NULL;
}
void checkFreshIdent(char *name) {
    Object * obj = findObject(symtab->currentScope->objList, name);
    if (obj != NULL)
        error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object* checkDeclaredIdent(char* name) {
    // Start search
    on_search = 0;

    Object *obj = lookupObject(name);

    // Reset search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredConstant(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_CONSTANT)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredType(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_TYPE)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredVariable(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_VARIABLE)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredFunction(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_FUNCTION)
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);

    return obj;
}

Object* checkDeclaredProcedure(char* name) {
   Object* obj = lookupObject(name);
  if (obj == NULL)
    error(ERR_UNDECLARED_PROCEDURE,currentToken->lineNo, currentToken->colNo);
  if (obj->kind != OBJ_PROCEDURE)
    error(ERR_INVALID_PROCEDURE,currentToken->lineNo, currentToken->colNo);
  return obj;
}

Object* checkDeclaredLValueIdent(char* name) {
    Object *obj = NULL;

    // Start search
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && (obj->kind == OBJ_FUNCTION || obj->kind == OBJ_PARAMETER || obj->kind == OBJ_VARIABLE))
            break;
    } while (obj != NULL);

    // End search
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}
void checkIntType(Type* type) {
  if (type->typeClass != TP_INT)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkCharType(Type* type) {
  if (type->typeClass != TP_CHAR)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkBasicType(Type* type) {
  if (type->typeClass != TP_INT && type->typeClass != TP_CHAR)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkArrayType(Type* type) {
  if (type->typeClass != TP_ARRAY)
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
}

void checkTypeEquality(Type* type1, Type* type2) {
  if (type1->typeClass != type2->typeClass) {
    error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
  } else if (type1->typeClass == TP_ARRAY) {
    checkTypeEquality(type1->elementType, type2->elementType);
    if (type1->arraySize != type2->arraySize)
      error(ERR_TYPE_INCONSISTENCY, currentToken->lineNo, currentToken->colNo);
  }
}