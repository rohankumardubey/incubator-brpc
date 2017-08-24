// bthread - A M:N threading library to make applications more concurrent.
// Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved

// Author: Ge,Jun (gejun@baidu.com)
// Date: Sun Sep  7 22:37:39 CST 2014

#ifndef BAIDU_BTHREAD_ALLOCATE_STACK_H
#define BAIDU_BTHREAD_ALLOCATE_STACK_H

#include <assert.h>
#include <gflags/gflags.h>          // DECLARE_int32
#include "bthread/types.h"
#include "bthread/context.h"        // bthread_fcontext_t
#include "base/object_pool.h"

namespace bthread {

struct StackStorage {
     int stacksize;
     int guardsize;
    // Assume stack grows upwards.
    // http://www.boost.org/doc/libs/1_55_0/libs/context/doc/html/context/stack.html
    void* bottom;
    unsigned valgrind_stack_id;

    // Clears all members.
    void zeroize() {
        stacksize = 0;
        guardsize = 0;
        bottom = NULL;
        valgrind_stack_id = 0;
    }
};
 
// Allocate a piece of stack.
int allocate_stack_storage(StackStorage* s, int stacksize, int guardsize);
// Deallocate a piece of stack. Parameters MUST be returned or set by the
// corresponding allocate_stack_storage() otherwise behavior is undefined.
void deallocate_stack_storage(StackStorage* s);

enum StackType {
    STACK_TYPE_MAIN = 0,
    STACK_TYPE_PTHREAD = BTHREAD_STACKTYPE_PTHREAD,
    STACK_TYPE_SMALL = BTHREAD_STACKTYPE_SMALL,
    STACK_TYPE_NORMAL = BTHREAD_STACKTYPE_NORMAL,
    STACK_TYPE_LARGE = BTHREAD_STACKTYPE_LARGE
};

struct ContextualStack {
    bthread_fcontext_t context;
    StackType stacktype;
    StackStorage storage;
};

// Get a stack in the `type' and run `entry' at the first time that the
// stack is jumped.
ContextualStack* get_stack(StackType type, void (*entry)(intptr_t));
// Recycle a stack. NULL does nothing.
void return_stack(ContextualStack*);
// Jump from stack `from' to stack `to'. `from' must be the stack of callsite
// (to save contexts before jumping)
void jump_stack(ContextualStack* from, ContextualStack* to);

}  // namespace bthread

#include "bthread/stack_inl.h"

#endif  // BAIDU_BTHREAD_ALLOCATE_STACK_H