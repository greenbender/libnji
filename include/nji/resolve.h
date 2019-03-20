#ifndef __NJI_CLASS_H__
#define __NJI_CLASS_H__


#include <jni.h>
#include "nji/error.h"


#ifdef __cplusplus
extern "C" {
#endif


#define ENTRIES(v) (sizeof(v)/sizeof(*(v)))


#define FLAG_STATIC 1
#define FLAG_OPTIONAL 2


/* class definition */
typedef struct {
	jclass Class;
	jfieldID *Fields;
	jmethodID *Constructors;
	jmethodID *Methods;
} ClassDef;


/* class symbols */
typedef struct {
	const char *name;
	const char *signature;
	int flags;
} FieldSym, MethodSym;


typedef struct {
	const char *signature;
	int flags;
} ConstructorSym;


typedef struct {
	const char *name;
	FieldSym *Fields;
	ConstructorSym *Constructors;
	MethodSym *Methods;
} ClassSym;


/* resolve class symbols into class definition */
nji_error resolveClassDef(ClassDef *def, ClassSym *sym);


#ifdef __cplusplus
}
#endif


#endif
