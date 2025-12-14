#ifndef SP_INTEROP_H
#define SP_INTEROP_H

#include <ultra64.h>

// Handle type for SourcePawn
typedef u32 Handle_t;
#define BAD_HANDLE 0

// Status codes
enum InteropStatus {
    INTEROP_OK = 0,
    INTEROP_BAD_HANDLE,
    INTEROP_NULL_POINTER,
    INTEROP_TYPE_MISMATCH
};

#ifdef __cplusplus
extern "C" {
#endif

// Safe Handle Management
Handle_t CreateHandleFromPtr(void *ptr);
void *GetPtrFromHandle(Handle_t handle);
void FreeHandle(Handle_t handle);

// Type-Safe Interface
enum InteropStatus SM64_GetMarioPosition(Handle_t marioHandle, float *x, float *y, float *z);
enum InteropStatus SM64_SetMarioAction(Handle_t marioHandle, u32 action);

#ifdef __cplusplus
}
#endif

#endif // SP_INTEROP_H
