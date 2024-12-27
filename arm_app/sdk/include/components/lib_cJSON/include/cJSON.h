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

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

#include "ty_cJSON.h"

/* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 15

#include <stddef.h>

/* cJSON Types: */
#define cJSON_Invalid  ty_cJSON_Invalid
#define cJSON_False    ty_cJSON_False
#define cJSON_True     ty_cJSON_True
#define cJSON_NULL     ty_cJSON_NULL
#define cJSON_Number   ty_cJSON_Number
#define cJSON_String   ty_cJSON_String
#define cJSON_Array    ty_cJSON_Array
#define cJSON_Object   ty_cJSON_Object
#define cJSON_Raw      ty_cJSON_Raw

#define cJSON_IsReference   ty_cJSON_IsReference
#define cJSON_StringIsConst ty_cJSON_StringIsConst

/* The ty_cJSON structure: */
/* The cJSON structure: */
typedef struct ty_cJSON cJSON;

typedef struct ty_cJSON_Hooks cJSON_Hooks;

#define cJSON_bool ty_cJSON_bool

/* returns the version of ty_cJSON as a string */
#define cJSON_Version ty_cJSON_Version

/* Supply malloc, realloc and free functions to ty_cJSON */
#define cJSON_InitHooks ty_cJSON_InitHooks

/* Memory Management: the caller is always responsible to free the results from all variants of ty_cJSON_Parse (with ty_cJSON_Delete) and ty_cJSON_Print (with stdlib free, ty_cJSON_Hooks.free_fn, or ty_cJSON_free as appropriate). The exception is ty_cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
/* Supply a block of JSON, and this returns a ty_cJSON object you can interrogate. */
#define cJSON_Parse ty_cJSON_Parse
#define cJSON_ParseWithLength ty_cJSON_ParseWithLength
/* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
/* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match ty_cJSON_GetErrorPtr(). */
#define cJSON_ParseWithOpts ty_cJSON_ParseWithOpts
#define cJSON_ParseWithLengthOpts ty_cJSON_ParseWithLengthOpts

/* Render a ty_cJSON entity to text for transfer/storage. */
#define cJSON_Print ty_cJSON_Print
/* Render a ty_cJSON entity to text for transfer/storage without any formatting. */
#define cJSON_PrintUnformatted ty_cJSON_PrintUnformatted
/* Render a ty_cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
#define cJSON_PrintBuffered ty_cJSON_PrintBuffered
/* Render a ty_cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
/* NOTE: ty_cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
#define cJSON_PrintPreallocated ty_cJSON_PrintPreallocated
/* Delete a ty_cJSON entity and all subentities. */
#define cJSON_Delete ty_cJSON_Delete

/* Returns the number of items in an array (or object). */
#define cJSON_GetArraySize ty_cJSON_GetArraySize
/* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
#define cJSON_GetArrayItem ty_cJSON_GetArrayItem
/* Get item "string" from object. Case insensitive. */
#define cJSON_GetObjectItem ty_cJSON_GetObjectItem
#define cJSON_GetObjectItemCaseSensitive ty_cJSON_GetObjectItemCaseSensitive
#define cJSON_HasObjectItem ty_cJSON_HasObjectItem
/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when ty_cJSON_Parse() returns 0. 0 when ty_cJSON_Parse() succeeds. */
#define cJSON_GetErrorPtr ty_cJSON_GetErrorPtr

/* Check item type and return its value */
#define cJSON_GetStringValue ty_cJSON_GetStringValue
#define cJSON_GetNumberValue ty_cJSON_GetNumberValue

/* These functions check the type of an item */
#define cJSON_IsInvalid ty_cJSON_IsInvalid
#define cJSON_IsFalse   ty_cJSON_IsFalse
#define cJSON_IsTrue    ty_cJSON_IsTrue
#define cJSON_IsBool    ty_cJSON_IsBool
#define cJSON_IsNull    ty_cJSON_IsNull
#define cJSON_IsNumber  ty_cJSON_IsNumber
#define cJSON_IsString  ty_cJSON_IsString
#define cJSON_IsArray   ty_cJSON_IsArray
#define cJSON_IsObject  ty_cJSON_IsObject
#define cJSON_IsRaw     ty_cJSON_IsRaw

/* These calls create a ty_cJSON item of the appropriate type. */
#define cJSON_CreateNull   ty_cJSON_CreateNull
#define cJSON_CreateTrue   ty_cJSON_CreateTrue
#define cJSON_CreateFalse  ty_cJSON_CreateFalse
#define cJSON_CreateBool   ty_cJSON_CreateBool
#define cJSON_CreateNumber ty_cJSON_CreateNumber
#define cJSON_CreateString ty_cJSON_CreateString
/* raw json */
#define cJSON_CreateRaw    ty_cJSON_CreateRaw
#define cJSON_CreateArray  ty_cJSON_CreateArray
#define cJSON_CreateObject ty_cJSON_CreateObject

/* Create a string where valuestring references a string so
 * it will not be freed by ty_cJSON_Delete */
#define cJSON_CreateStringReference ty_cJSON_CreateStringReference
/* Create an object/array that only references it's elements so
 * they will not be freed by ty_cJSON_Delete */
#define cJSON_CreateObjectReference ty_cJSON_CreateObjectReference
#define cJSON_CreateArrayReference  ty_cJSON_CreateArrayReference

/* These utilities create an Array of count items.
 * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
#define cJSON_CreateIntArray    ty_cJSON_CreateIntArray
#define cJSON_CreateFloatArray  ty_cJSON_CreateFloatArray
#define cJSON_CreateDoubleArray ty_cJSON_CreateDoubleArray
#define cJSON_CreateStringArray ty_cJSON_CreateStringArray

/* Append item to the specified array/object. */
#define cJSON_AddItemToArray  ty_cJSON_AddItemToArray
#define cJSON_AddItemToObject ty_cJSON_AddItemToObject
/* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the ty_cJSON object.
 * WARNING: When this function was used, make sure to always check that (item->type & ty_cJSON_StringIsConst) is zero before
 * writing to `item->string` */
#define cJSON_AddItemToObjectCS ty_cJSON_AddItemToObjectCS
/* Append reference to item to the specified array/object. Use this when you want to add an existing ty_cJSON to a new ty_cJSON, but don't want to corrupt your existing ty_cJSON. */
#define cJSON_AddItemReferenceToArray  ty_cJSON_AddItemReferenceToArray
#define cJSON_AddItemReferenceToObject ty_cJSON_AddItemReferenceToObject

/* Remove/Detach items from Arrays/Objects. */
#define cJSON_DetachItemViaPointer ty_cJSON_DetachItemViaPointer
#define cJSON_DetachItemFromArray  ty_cJSON_DetachItemFromArray
#define cJSON_DeleteItemFromArray  ty_cJSON_DeleteItemFromArray
#define cJSON_DetachItemFromObject ty_cJSON_DetachItemFromObject
#define cJSON_DetachItemFromObjectCaseSensitive ty_cJSON_DetachItemFromObjectCaseSensitive
#define cJSON_DeleteItemFromObject ty_cJSON_DeleteItemFromObject
#define cJSON_DeleteItemFromObjectCaseSensitive ty_cJSON_DeleteItemFromObjectCaseSensitive

/* Update array items. */
#define cJSON_InsertItemInArray ty_cJSON_InsertItemInArray
#define cJSON_ReplaceItemViaPointer ty_cJSON_ReplaceItemViaPointer
#define cJSON_ReplaceItemInArray ty_cJSON_ReplaceItemInArray
#define cJSON_ReplaceItemInObject ty_cJSON_ReplaceItemInObject
#define cJSON_ReplaceItemInObjectCaseSensitive ty_cJSON_ReplaceItemInObjectCaseSensitive

/* Duplicate a ty_cJSON item */
#define cJSON_Duplicate ty_cJSON_Duplicate
/* Duplicate will create a new, identical ty_cJSON item to the one you pass, in new memory that will
 * need to be released. With recurse!=0, it will duplicate any children connected to the item.
 * The item->next and ->prev pointers are always zero on return from Duplicate. */
/* Recursively compare two ty_cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
 * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
#define cJSON_Compare ty_cJSON_Compare

/* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
 * The input pointer json cannot point to a read-only address area, such as a string constant, 
 * but should point to a readable and writable address area. */
#define cJSON_Minify ty_cJSON_Minify

/* Helper functions for creating and adding items to an object at the same time.
 * They return the added item or NULL on failure. */
#define cJSON_AddNullToObject   ty_cJSON_AddNullToObject
#define cJSON_AddTrueToObject   ty_cJSON_AddTrueToObject
#define cJSON_AddFalseToObject  ty_cJSON_AddFalseToObject
#define cJSON_AddBoolToObject   ty_cJSON_AddBoolToObject
#define cJSON_AddNumberToObject ty_cJSON_AddNumberToObject
#define cJSON_AddStringToObject ty_cJSON_AddStringToObject
#define cJSON_AddRawToObject    ty_cJSON_AddRawToObject
#define cJSON_AddObjectToObject ty_cJSON_AddObjectToObject
#define cJSON_AddArrayToObject  ty_cJSON_AddArrayToObject

/* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue     ty_cJSON_SetIntValue
/* helper for the ty_cJSON_SetNumberValue macro */
#define cJSON_SetNumberHelper ty_cJSON_SetNumberHelper
#define cJSON_SetNumberValue  ty_cJSON_SetNumberValue
/* Change the valuestring of a ty_cJSON_String object, only takes effect when type of object is ty_cJSON_String */
#define cJSON_SetValuestring  ty_cJSON_SetValuestring

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach    ty_cJSON_ArrayForEach

/* malloc/free objects using the malloc/free functions that have been set with ty_cJSON_InitHooks */
#define cJSON_malloc ty_cJSON_malloc
#define cJSON_free   ty_cJSON_free

#ifdef __cplusplus
}
#endif

#endif
