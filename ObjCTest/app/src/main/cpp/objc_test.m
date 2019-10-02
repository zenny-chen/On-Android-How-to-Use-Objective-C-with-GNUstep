#import <Foundation/Foundation.h>
#include "zf_sys/zf_sys.h"

#define NSLog(...)   (void)({NSString *s = [NSString stringWithFormat:__VA_ARGS__]; \
                            puts(s.UTF8String);})


#ifdef __OBJC__

#warning This is an Objective-C source file!!

#endif


void ObjCTest(void)
{
    @autoreleasepool {
        NSArray<NSNumber*> *array = @[ @10, @20, @30 ];

        int sum = 0;
        for(NSNumber *num in array)
            sum += num.intValue;

        // 输出：array size is: 3, sum = 60
        NSLog(@"array size is: %tu, sum = %d", array.count, sum);

    }
}

