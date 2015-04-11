#ifndef CSON__h
#define CSON__h


/* CSON Types: */
#define CSON_False 0
#define CSON_True 1
#define CSON_NULL 2
#define CSON_Number 3
#define CSON_String 4
#define CSON_Array 5
#define CSON_Object 6
	
#define CSON_IsReference 256

/* The CSON structure: */
typedef struct CSON {
	struct CSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct CSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==CSON_String */
	int valueint;				/* The item's number, if type==CSON_Number */
	double valuedouble;			/* The item's number, if type==CSON_Number */

	char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} CSON;

typedef struct CSON_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} CSON_Hooks;

/* Supply malloc, realloc and free functions to CSON */
extern void CSON_InitHooks(CSON_Hooks* hooks);


/* Supply a block of JSON, and this returns a CSON object you can interrogate. Call CSON_Delete when finished. */
extern CSON *CSON_Parse(const char *value);
/* Render a CSON entity to text for transfer/storage. Free the char* when finished. */
extern char  *CSON_Print(CSON *item);
/* Render a CSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *CSON_PrintUnformatted(CSON *item);
/* Delete a CSON entity and all subentities. */
extern void   CSON_Delete(CSON *c);

/* Returns the number of items in an array (or object). */
extern int	  CSON_GetArraySize(CSON *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern CSON *CSON_GetArrayItem(CSON *array,int item);
/* Get item "string" from object. Case insensitive. */
extern CSON *CSON_GetObjectItem(CSON *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when CSON_Parse() returns 0. 0 when CSON_Parse() succeeds. */
extern const char *CSON_GetErrorPtr();
	
/* These calls create a CSON item of the appropriate type. */
extern CSON *CSON_CreateNull();
extern CSON *CSON_CreateTrue();
extern CSON *CSON_CreateFalse();
extern CSON *CSON_CreateBool(int b);
extern CSON *CSON_CreateNumber(double num);
extern CSON *CSON_CreateString(const char *string);
extern CSON *CSON_CreateArray();
extern CSON *CSON_CreateObject();

/* These utilities create an Array of count items. */
extern CSON *CSON_CreateIntArray(int *numbers,int count);
extern CSON *CSON_CreateFloatArray(float *numbers,int count);
extern CSON *CSON_CreateDoubleArray(double *numbers,int count);
extern CSON *CSON_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void CSON_AddItemToArray(CSON *array, CSON *item);
extern void	CSON_AddItemToObject(CSON *object,const char *string,CSON *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing CSON to a new CSON, but don't want to corrupt your existing CSON. */
extern void CSON_AddItemReferenceToArray(CSON *array, CSON *item);
extern void	CSON_AddItemReferenceToObject(CSON *object,const char *string,CSON *item);

/* Remove/Detatch items from Arrays/Objects. */
extern CSON *CSON_DetachItemFromArray(CSON *array,int which);
extern void   CSON_DeleteItemFromArray(CSON *array,int which);
extern CSON *CSON_DetachItemFromObject(CSON *object,const char *string);
extern void   CSON_DeleteItemFromObject(CSON *object,const char *string);
	
/* Update array items. */
extern void CSON_ReplaceItemInArray(CSON *array,int which,CSON *newitem);
extern void CSON_ReplaceItemInObject(CSON *object,const char *string,CSON *newitem);

#define CSON_AddNullToObject(object,name)	CSON_AddItemToObject(object, name, CSON_CreateNull())
#define CSON_AddTrueToObject(object,name)	CSON_AddItemToObject(object, name, CSON_CreateTrue())
#define CSON_AddFalseToObject(object,name)		CSON_AddItemToObject(object, name, CSON_CreateFalse())
#define CSON_AddNumberToObject(object,name,n)	CSON_AddItemToObject(object, name, CSON_CreateNumber(n))
#define CSON_AddStringToObject(object,name,s)	CSON_AddItemToObject(object, name, CSON_CreateString(s))

#define CSON_AddNumberToArray(array,num)    CSON_AddItemToArray(array, CSON_CreateNumber(num))


#endif
