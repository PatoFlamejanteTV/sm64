#include "sp_interop.h"
#include <map>
#include <mutex>

extern "C" {
#include "sm64.h"
}

// Global handle map to prevent passing raw pointers to SourcePawn
// Key: Handle_t (u32), Value: void*
static std::map<Handle_t, void*> g_HandleMap;
static Handle_t g_NextHandle = 1;
static std::mutex g_HandleMutex;

extern "C" {

// Add a reverse map at file scope
static std::map<void*, Handle_t> g_PtrToHandleMap;

Handle_t CreateHandleFromPtr(void *ptr) {
    if (!ptr) return BAD_HANDLE;
    std::lock_guard<std::mutex> lock(g_HandleMutex);

    // O(log n) lookup for existing pointer
    auto it = g_PtrToHandleMap.find(ptr);
    if (it != g_PtrToHandleMap.end()) {
        return it->second;
    }

    Handle_t handle = g_NextHandle++;
    g_HandleMap[handle] = ptr;
    g_PtrToHandleMap[ptr] = handle;
    return handle;
}

void *GetPtrFromHandle(Handle_t handle) {
    std::lock_guard<std::mutex> lock(g_HandleMutex);
    auto it = g_HandleMap.find(handle);
    if (it != g_HandleMap.end()) {
        return it->second;
    }
    return nullptr;
}

void FreeHandle(Handle_t handle) {
    std::lock_guard<std::mutex> lock(g_HandleMutex);
    g_HandleMap.erase(handle);
}

// Native Implementation Example
// Check strict for memory leaks, type safety, and null pointers

enum InteropStatus SM64_GetMarioPosition(Handle_t marioHandle, float *x, float *y, float *z) {
    if (!x || !y || !z) {
        return INTEROP_NULL_POINTER;
    }

    struct MarioState *m = (struct MarioState *)GetPtrFromHandle(marioHandle);
    if (!m) {
        return INTEROP_BAD_HANDLE;
    }

    // Safety: Pointers are valid, Handle is resolved.
    *x = m->pos[0];
    *y = m->pos[1];
    *z = m->pos[2];

    return INTEROP_OK;
}

enum InteropStatus SM64_SetMarioAction(Handle_t marioHandle, u32 action) {
    struct MarioState *m = (struct MarioState *)GetPtrFromHandle(marioHandle);
    if (!m) {
        return INTEROP_BAD_HANDLE;
    }

    // Basic validation of action (hypothetical)
    // if (action > MAX_ACTION_ID) return INTEROP_TYPE_MISMATCH;

    m->action = action;
    return INTEROP_OK;
}

} // extern "C"
