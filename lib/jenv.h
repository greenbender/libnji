#ifndef __NJI_JENV_H__
#define __NJI_JENV_H__


#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif


JNIEnv *get_jenv(void);
void close_jenv(void);


#ifdef __cplusplus
}
#endif


#endif
