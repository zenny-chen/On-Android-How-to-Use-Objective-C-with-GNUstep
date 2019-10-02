#include <jni.h>
#include "zf_sys/zf_sys.h"


extern void ObjCTest(void);

JNIEXPORT jstring JNICALL
Java_com_codelearning_objctest_MainActivity_stringFromJNI(JNIEnv *env, jobject this)
{

    ObjCTest();

    const char *hello = "Hello from C";
    return (*env)->NewStringUTF(env, hello);
}

