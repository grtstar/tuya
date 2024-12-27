/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef ty_cJSON__h
#define ty_cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__

/* When compiling for windows, we specify a specific calling convention to avoid issues where we are being called from a project with a different default calling convention.  For windows you have 3 define options:

CJSON_HIDE_SYMBOLS - Define this in the case where you don't want to ever dllexport symbols
CJSON_EXPORT_SYMBOLS - Define this on library build when you want to dllexport symbols (default)
CJSON_IMPORT_SYMBOLS - Define this if you want to dllimport symbol

For *nix builds that support visibility attribute, you can define similar behavior by

setting default visibility to hidden by adding
-fvisibility=hidden (for gcc)
or
-xldscope=hidden (for sun cc)
to CFLAGS

then using the CJSON_API_VISIBILITY flag to "export" the same symbols the way CJSON_EXPORT_SYMBOLS does

*/

#define CJSON_CDECL __cdecl
#define CJSON_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(CJSON_HIDE_SYMBOLS) && !defined(CJSON_IMPORT_SYMBOLS) && !defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_EXPORT_SYMBOLS
#endif

#if defined(CJSON_HIDE_SYMBOLS)
#define CJSON_PUBLIC(type)   type CJSON_STDCALL
#elif defined(CJSON_EXPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllexport) type CJSON_STDCALL
#elif defined(CJSON_IMPORT_SYMBOLS)
#define CJSON_PUBLIC(type)   __declspec(dllimport) type CJSON_STDCALL
#endif
#else /* !__WINDOWS__ */
#define CJSON_CDECL
#define CJSON_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define CJSON_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define CJSON_PUBLIC(type) type
#endif
#endif

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 15

#include <stddef.h>

/* cJSON Types: */
#define ty_cJSON_Invalid (0)
#define ty_cJSON_False  (1 << 0)
#define ty_cJSON_True   (1 << 1)
#define ty_cJSON_NULL   (1 << 2)
#define ty_cJSON_Number (1 << 3)
#define ty_cJSON_String (1 << 4)
#define ty_cJSON_Array  (1 << 5)
#define ty_cJSON_Object (1 << 6)
#define ty_cJSON_Raw    (1 << 7) /* raw json */

#define ty_cJSON_IsReference 256
#define ty_cJSON_StringIsConst 512

/* The ty_cJSON structure: */
typedef struct ty_cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct ty_cJSON *next;
    struct ty_cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct ty_cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==ty_cJSON_String  and type == ty_cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use ty_cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==ty_cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;
} ty_cJSON;

typedef struct ty_cJSON_Hooks
{
      /* malloc/free are CDECL on Windows regardless of the default calling convention of the compiler, so ensure the hooks allow passing those functions directly. */
      void *(CJSON_CDECL *malloc_fn)(size_t sz);
      void (CJSON_CDECL *free_fn)(void *ptr);
} ty_cJSON_Hooks;

typedef int ty_cJSON_bool;

/* Limits how deeply nested arrays/objects can be before ty_cJSON rejects to parse them.
 * This is to prevent stack overflows. */
#ifndef CJSON_NESTING_LIMIT
#define CJSON_NESTING_LIMIT 1000
#endif

/* returns the version of ty_cJSON as a string */
CJSON_PUBLIC(const char*) ty_cJSON_Version(void);

/* Supply malloc, realloc and free functions to ty_cJSON */
CJSON_PUBLIC(void) ty_cJSON_InitHooks(ty_cJSON_Hooks* hooks);

/* Memory Management: the caller is always responsible to free the results from all variants of ty_cJSON_Parse (with ty_cJSON_Delete) and ty_cJSON_Print (with stdlib free, ty_cJSON_Hooks.free_fn, or ty_cJSON_free as appropriate). The exception is ty_cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a ty_cJSON object you can interrogate. */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_Parse(const char *value);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_ParseWithLength(const char *value, size_t buffer_length);
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match ty_cJSON_GetErrorPtr(). */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_ParseWithOpts(const char *value, const char **return_parse_end, ty_cJSON_bool require_null_terminated);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, ty_cJSON_bool require_null_terminated);

/* Render a ty_cJSON entity to text for transfer/storage. */
CJSON_PUBLIC(char *) ty_cJSON_Print(const ty_cJSON *item);
/* Render a ty_cJSON entity to text for transfer/storage without any formatting. */
CJSON_PUBLIC(char *) ty_cJSON_PrintUnformatted(const ty_cJSON *item);
/* Render a ty_cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
CJSON_PUBLIC(char *) ty_cJSON_PrintBuffered(const ty_cJSON *item, int prebuffer, ty_cJSON_bool fmt);
/* Render a ty_cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: ty_cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_PrintPreallocated(ty_cJSON *item, char *buffer, const int length, const ty_cJSON_bool format);
/* Delete a ty_cJSON entity and all subentities. */
CJSON_PUBLIC(void) ty_cJSON_Delete(ty_cJSON *item);
CJSON_PUBLIC(void) ty_cJSON_FreeBuffer(char *buffer);

/* Returns the number of items in an array (or object). */
CJSON_PUBLIC(int) ty_cJSON_GetArraySize(const ty_cJSON *array);
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_GetArrayItem(const ty_cJSON *array, int index);
/* Get item "string" from object. Case insensitive. */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_GetObjectItem(const ty_cJSON * const object, const char * const string);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_GetObjectItemCaseSensitive(const ty_cJSON * const object, const char * const string);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_HasObjectItem(const ty_cJSON *object, const char *string);
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when ty_cJSON_Parse() returns 0. 0 when ty_cJSON_Parse() succeeds. */
CJSON_PUBLIC(const char *) ty_cJSON_GetErrorPtr(void);

/* Check item type and return its value */
CJSON_PUBLIC(char *) ty_cJSON_GetStringValue(const ty_cJSON * const item);
CJSON_PUBLIC(double) ty_cJSON_GetNumberValue(const ty_cJSON * const item);

/* These functions check the type of an item */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsInvalid(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsFalse(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsTrue(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsBool(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsNull(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsNumber(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsString(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsArray(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsObject(const ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_IsRaw(const ty_cJSON * const item);

/* These calls create a ty_cJSON item of the appropriate type. */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateNull(void);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateTrue(void);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateFalse(void);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateBool(ty_cJSON_bool boolean);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateNumber(double num);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateString(const char *string);
/* raw json */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateRaw(const char *raw);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateArray(void);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateObject(void);

/* Create a string where valuestring references a string so
 * it will not be freed by ty_cJSON_Delete */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateStringReference(const char *string);
/* Create an object/array that only references it's elements so
 * they will not be freed by ty_cJSON_Delete */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateObjectReference(const ty_cJSON *child);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateArrayReference(const ty_cJSON *child);

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateIntArray(const int *numbers, int count);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateFloatArray(const float *numbers, int count);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateDoubleArray(const double *numbers, int count);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_CreateStringArray(const char *const *strings, int count);

/* Append item to the specified array/object. */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_AddItemToArray(ty_cJSON *array, ty_cJSON *item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_AddItemToObject(ty_cJSON *object, const char *string, ty_cJSON *item);
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the ty_cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & ty_cJSON_StringIsConst) is zero before
 * writing to `item->string` */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_AddItemToObjectCS(ty_cJSON *object, const char *string, ty_cJSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing ty_cJSON to a new ty_cJSON, but don't want to corrupt your existing ty_cJSON. */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_AddItemReferenceToArray(ty_cJSON *array, ty_cJSON *item);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_AddItemReferenceToObject(ty_cJSON *object, const char *string, ty_cJSON *item);

/* Remove/Detach items from Arrays/Objects. */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_DetachItemViaPointer(ty_cJSON *parent, ty_cJSON * const item);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_DetachItemFromArray(ty_cJSON *array, int which);
CJSON_PUBLIC(void) ty_cJSON_DeleteItemFromArray(ty_cJSON *array, int which);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_DetachItemFromObject(ty_cJSON *object, const char *string);
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_DetachItemFromObjectCaseSensitive(ty_cJSON *object, const char *string);
CJSON_PUBLIC(void) ty_cJSON_DeleteItemFromObject(ty_cJSON *object, const char *string);
CJSON_PUBLIC(void) ty_cJSON_DeleteItemFromObjectCaseSensitive(ty_cJSON *object, const char *string);

/* Update array items. */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_InsertItemInArray(ty_cJSON *array, int which, ty_cJSON *newitem); /* Shifts pre-existing items to the right. */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_ReplaceItemViaPointer(ty_cJSON * const parent, ty_cJSON * const item, ty_cJSON * replacement);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_ReplaceItemInArray(ty_cJSON *array, int which, ty_cJSON *newitem);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_ReplaceItemInObject(ty_cJSON *object,const char *string,ty_cJSON *newitem);
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_ReplaceItemInObjectCaseSensitive(ty_cJSON *object,const char *string,ty_cJSON *newitem);

/* Duplicate a ty_cJSON item */
CJSON_PUBLIC(ty_cJSON *) ty_cJSON_Duplicate(const ty_cJSON *item, ty_cJSON_bool recurse);
/* Duplicate will create a new, identical ty_cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two ty_cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
CJSON_PUBLIC(ty_cJSON_bool) ty_cJSON_Compare(const ty_cJSON * const a, const ty_cJSON * const b, const ty_cJSON_bool case_sensitive);

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
CJSON_PUBLIC(void) ty_cJSON_Minify(char *json);

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddNullToObject(ty_cJSON * const object, const char * const name);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddTrueToObject(ty_cJSON * const object, const char * const name);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddFalseToObject(ty_cJSON * const object, const char * const name);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddBoolToObject(ty_cJSON * const object, const char * const name, const ty_cJSON_bool boolean);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddNumberToObject(ty_cJSON * const object, const char * const name, const double number);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddStringToObject(ty_cJSON * const object, const char * const name, const char * const string);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddRawToObject(ty_cJSON * const object, const char * const name, const char * const raw);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddObjectToObject(ty_cJSON * const object, const char * const name);
CJSON_PUBLIC(ty_cJSON*) ty_cJSON_AddArrayToObject(ty_cJSON * const object, const char * const name);

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define ty_cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
/* helper for the ty_cJSON_SetNumberValue macro */
CJSON_PUBLIC(double) ty_cJSON_SetNumberHelper(ty_cJSON *object, double number);
#define ty_cJSON_SetNumberValue(object, number) ((object != NULL) ? ty_cJSON_SetNumberHelper(object, (double)number) : (number))
/* Change the valuestring of a ty_cJSON_String object, only takes effect when type of object is ty_cJSON_String */
CJSON_PUBLIC(char*) ty_cJSON_SetValuestring(ty_cJSON *object, const char *valuestring);

/* Macro for iterating over an array or object */
#define ty_cJSON_ArrayForEach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

/* malloc/free objects using the malloc/free functions that have been set with ty_cJSON_InitHooks */
CJSON_PUBLIC(void *) ty_cJSON_malloc(size_t size);
CJSON_PUBLIC(void) ty_cJSON_free(void *object);

#ifdef __cplusplus
}
#endif

#endif
