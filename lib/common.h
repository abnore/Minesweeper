#ifndef COMMON_H
#define COMMON_H

//------------------------------------------------------------------------------
// Common utility header for Canopy — memory, input, logging, and timing
//------------------------------------------------------------------------------

#ifndef CUSTOM_ALLOCATOR
    #include <stdlib.h>   ///< Fallback to standard allocator if custom not defined
#endif

#include <stdio.h>        ///< File I/O (FILE, fopen, fread, fclose)
#include <string.h>       ///< Memory functions (memcpy, memset, etc.)
#include <stdint.h>       ///< Integer types (uint8_t, uint32_t, etc.)
#include <stdbool.h>      ///< Boolean type

#include "input.h"        ///< Input keys and mouse buttons
#include "logger.h"       ///< Logging system
#include "canopy_time.h"  ///< Timing utilities

//------------------------------------------------------------------------------
// Memory Allocation
//------------------------------------------------------------------------------

/**
 * @brief Allocate zero-initialized memory.
 *
 * @param count Number of elements.
 * @param size Size of each element.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void *canopy_calloc(size_t count, size_t size);

/**
 * @brief Free allocated memory.
 *
 * @param ptr Pointer to memory to free.
 */
void canopy_free(void *ptr);

/**
 * @brief Allocate memory.
 *
 * @param size Size in bytes.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void *canopy_malloc(size_t size);

/**
 * @brief Reallocate memory.
 *
 * @param ptr Pointer to previously allocated memory.
 * @param size New size in bytes.
 * @return Pointer to reallocated memory, or NULL on failure.
 */
void *canopy_realloc(void *ptr, size_t size);

//------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------

/**
 * @brief Convert a keyboard key to a human-readable string.
 *
 * @param key The key enum value (e.g., CANOPY_KEY_A).
 * @return Pointer to a string representation (e.g., "a", "space", etc.).
 */
const char* canopy_key_to_string(keys key);

#endif // COMMON_H
