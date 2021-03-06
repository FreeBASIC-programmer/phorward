/* -MODULE----------------------------------------------------------------------
Phorward Foundation Toolkit
Copyright (C) 2006-2018 by Phorward Software Technologies, Jan Max Meyer
http://www.phorward-software.com ++ contact<at>phorward<dash>software<dot>com
All rights reserved. See LICENSE for more information.

File:	array.c
Author:	Jan Max Meyer
Usage:	Universal, dynamic array management functions.
----------------------------------------------------------------------------- */

#include "phorward.h"

#define STD_CHUNK			128		/* Default chunk size */

/** Performs an array initialization.

//array// is the pointer to the array to be initialized.

//size// defines the size of one array element, in bytes.
This should be evaluated using the sizeof()-macro.

//chunk// defines the chunk size, where an array-(re)allocation will be performed.
If, e.g. this is set to 128, then, if the 128th item is created within the
array, a realloction is done. Once allocated memory remains until the array is
freed again.
*/
pboolean parray_init( parray* array, size_t size, size_t chunk )
{
	PROC( "parray_init" );
	PARMS( "array", "%p", array );
	PARMS( "size", "%ld", size );
	PARMS( "chunk", "%ld", chunk );

	if( !( array && size > 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( chunk <= 0 )
		chunk = STD_CHUNK;

	memset( array, 0, sizeof( parray ) );
	array->size = size;
	array->chunk = chunk;

	RETURN( TRUE );
}

/** Create a new parray as an object with an element allocation size //size//
and a reallocation-chunk-size of //chunk//.

The returned memory must be released with parray_free().  */
parray* parray_create( size_t size, size_t chunk )
{
	parray*	array;

	if( size <= 0 )
	{
		WRONGPARAM;
		return (parray*)NULL;
	}

	array = (parray*)pmalloc( sizeof( parray ) );
	parray_init( array, size, chunk );

	return array;
}

/** Erase a dynamic array.

The array must not be reinitialized after destruction, using parray_init().

//array// is the pointer to the array to be erased. */
pboolean parray_erase( parray* array )
{
	PROC( "parray_free" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	array->array = pfree( array->array );
	array->count = array->first = array->last = 0;

	RETURN( TRUE );
}

/** Releases all the memory //array// uses and destroys the array object.

The function always returns (parray*)NULL. */
parray* parray_free( parray* array )
{
	if( !array )
		return (parray*)NULL;

	parray_erase( array );
	pfree( array );

	return (parray*)NULL;
}

/** Appends an element to the end of the array.

The element's memory is copied during the push. The item must be of the same
memory size as used at array initialization.

//array// is the pointer to array where to push an item on.

//item// is the pointer to the memory of the item that should be pushed onto the
array. The caller should cast his type into void, or wrap the push-operation
with a macro. It can be left (void*)NULL, so no memory will be copied.

The function returns the address of the newly pushed item, and (void*)NULL if
the item could not be pushed.
*/
void* parray_push( parray* array, void* item )
{
	void*	ptr;

	PROC( "parray_push" );
	PARMS( "array", "%p", array );
	PARMS( "item", "%p", item );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Is reallocation required? */
	if( !array->count || array->last == array->count )
	{
		array->count += array->chunk;

		if( !( array->array = (void*)prealloc(
				(void*)array->array, array->count * array->size ) ) )
			RETURN( (void*)NULL );
	}

	ptr = (char*)array->array + ( ( array->last++ ) * array->size );

	/* Copy item into last of array */
	if( item )
		memcpy( ptr, item, array->size );

	RETURN( ptr );
}

/** Reserves memory for //n// items in //array//.

This function is only used to assume that no memory reallocation is done when
the next //n// items are inserted/malloced. */
pboolean parray_reserve( parray* array, size_t n )
{
	PROC( "parray_reserve" );
	PARMS( "array", "%p", array );
	PARMS( "n", "%ld", n );

	if( !( array && n > 0 ) )
	{
		WRONGPARAM;
		RETURN( FALSE );
	}

	if( array->last + n < array->count )
		RETURN( TRUE );

	array->count += n + ( n % array->chunk );

	if( !( array->array = (void*)prealloc(
			(void*)array->array, array->count * array->size ) ) )
		RETURN( FALSE );

	RETURN( TRUE );
}

/** Pushes and "allocates" an empty element on the array.

This function is just a shortcut to ```parray_push( array, (void*)NULL )```,
and the memory of the pushed element is initialized to zero. */
void* parray_malloc( parray* array )
{
	void*	ptr;

	PROC( "parray_malloc" );
	PARMS( "array", "%p", array );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( !( ptr = parray_push( array, (void*)NULL ) ) )
		return ptr;

	memset( ptr, 0, array->size );
	return ptr;
}

/** Unshifts and "allocates" an empty element on the array.

This function is just a shortcut to ```parray_unshift( array, (void*)NULL )```,
and the memory of the unshifted element is initialized to zero. */
void* parray_rmalloc( parray* array )
{
	void*	ptr;

	PROC( "parray_rmalloc" );
	PARMS( "array", "%p", array );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( !( ptr = parray_unshift( array, (void*)NULL ) ) )
		return ptr;

	memset( ptr, 0, array->size );
	return ptr;
}

/** Insert item //item// at //offset// into array //array//.
Items right to //offset// will move up.

Gap space between the offset is filled with zero elements;
Handle with care! */
void* parray_insert( parray* array, size_t offset, void* item )
{
	void*	slot;

	PROC( "parray_insert" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%ld", offset );
	PARMS( "item", "%p", item );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Within current bounds? */
	if( array->first + offset < array->last )
	{
		MSG( "offset within bounds, inserting" );

		/* Allocate one item for moving up */
		parray_malloc( array );

		slot = (char*)array->array + ( array->first + offset ) * array->size;

		/* Move up existing items right of offset */
		memmove( slot + array->size, slot,
					( array->last - 1 -
						( array->first + offset ) ) * array->size );

		/* Put new element */
		if( item )
			memcpy( slot, item, array->size );
		else
			memset( slot, 0, array->size );

		RETURN( slot );
	}

	while( array->first + offset >= array->last )
	{
		if( !( slot = parray_malloc( array ) ) )
		{
			MSG( "Out of mem?" );
			RETURN( slot );
		}
	}

	if( item )
		memcpy( slot, item, array->size );

	RETURN( slot );
}

/** Remove item on //offset// from array //array//.

The removed item will be copied into //item//, if //item// is not NULL.
The function returns the memory of the removed item (it will contain the
moved up data part or invalid memory, if on the end). */
void* parray_remove( parray* array, size_t offset, void** item )
{
	void*	slot;

	PROC( "parray_remove" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%ld", offset );
	PARMS( "item", "%p", item );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Within current bounds? */
	if( array->first + offset >= array->last )
	{
		MSG( "Index out of bounds." );
		RETURN( (void*)NULL );
	}

	slot = (char*)array->array + ( array->first + offset ) * array->size;

	if( item )
		memcpy( *item, slot, ( array->first + offset ) * array->size );

	/* Move existing items to the left */
	if( array->first + offset + 1 < array->last )
		memmove( slot, slot + array->size,
				( array->last - ( array->first + offset + 1 ) )
					* array->size );

	array->last--;

	RETURN( slot );
}

/** Removes an element from the end of an array.

The function returns the pointer of the popped item. Because dynamic arrays only
grow and no memory is freed, the returned data pointer is still valid, and will
only be overidden with the next push operation.

//array// is the pointer to array where to pop an item off.

The function returns the address of the popped item, and (void*)NULL if the
item could not be popped (e.g. array is empty).
*/
void* parray_pop( parray* array )
{
	PROC( "parray_pop" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->last == array->first )
	{
		MSG( "last is zero, no items on the array." );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)array->array + ( ( --array->last ) * array->size ) );
}

/** Appends an element to the begin of the array.

The elements memory is copied during the unshift. The item must be of the same
memory size as used at array initialization.

//array// is the pointer to array where to push an item to the beginning.

//item// is the pointer to the memory of the item that should be pushed onto the
array. The caller should cast his type into void, or wrap the push-operation
with a macro. It can be left (void*)NULL, so no memory will be copied.

The function returns the address of the newly unhshifted item, and (void*)NULL
if the item could not be unshifted.
*/
void* parray_unshift( parray* array, void* item )
{
	void*	ptr;

	PROC( "parray_unshift" );
	PARMS( "array", "%p", array );
	PARMS( "item", "%p", item );

	if( !( array ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	/* Is reallocation required? */
	if( !array->count || array->first == 0 )
	{
		array->count += array->chunk;

		if( !( array->array = (void*)prealloc(
				(void*)array->array, array->count * array->size ) ) )
			RETURN( (void*)NULL );

		array->first = array->chunk;

		if( array->last > 0 )
			memmove( (char*)array->array + array->first * array->size,
						array->array, array->last * array->size );

		array->last += array->first;
	}

	ptr = (char*)array->array + ( --array->first * array->size );

	/* Copy item into last of array */
	if( item )
		memcpy( ptr, item, array->size );

	RETURN( ptr );
}

/** Removes an element from the begin of an array.

The function returns the pointer of the shifted item.
Because dynamic arrays only grow and no memory is freed, the returned data
pointer is still valid, and will only be overidden with the next unshift
operation.

//array// is the pointer to array where to pop an item off.

The function returns the address of the shifted item, and (void*)NULL if the
item could not be popped (e.g. array is empty).
*/
void* parray_shift( parray* array )
{
	PROC( "parray_shift" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->last == array->first )
	{
		MSG( "last is zero, no items on the array." );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)array->array + array->first++ * array->size );
}

/** Access an element from the array by its offset position from the left.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and (void*)NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the last of array).

Use parray_rget() for access items from the end.
*/
void* parray_get( parray* array, size_t offset )
{
	PROC( "parray_get" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->last == array->first
			|| offset >= ( array->last - array->first ) )
	{
		MSG( "offset defines an item that is out of bounds of the array" );
		RETURN( (void*)NULL );
	}

	RETURN( (char*)array->array + ( array->first + offset ) * array->size );
}

/** Put an element //item// at position //offset// of array //array//.

//array// is the pointer to array where to put the element to.
//offset// is the offset of the element to be set.
//item// is a pointer to the memory that will be copied into the
position at //offset//. If this is NULL, the position at //offset// will be
set to zero.

Returns the address of the item in the array, or NULL if the desired offset
is out of the array bounds.
*/
void* parray_put( parray* array, size_t offset, void* item )
{
	void*	slot;

	PROC( "parray_put" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );
	PARMS( "item", "%p", item );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->last == array->first
			|| offset >= ( array->last - array->first ) )
	{
		MSG( "Index out of bounds" );
		RETURN( (void*)NULL );
	}

	slot = (char*)array->array + ( array->first + offset ) * array->size;

	if( item )
		memcpy( slot, item, array->size );
	else
		memset( slot, 0, array->size );

	RETURN( slot );
}

/** Access an element from the array by its offset position from the right.

//array// is the pointer to array where to access the element from.
//offset// is the offset of the element to be accessed from the array's
base address.

Returns the address of the accessed item, and (void*)NULL if the item could not
be accessed (e.g. if the array is empty or offset is beyond the bottom of
the array).

Use parray_get() for access items from the begin.
*/
void* parray_rget( parray* array, size_t offset )
{
	PROC( "parray_rget" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	RETURN( parray_get( array, array->last - offset ) );
}

/** Put an element //item// at position //offset// from the right of
array //array//.

//array// is the pointer to array where to put the element to.
//offset// is the offset of the element to be set.
//item// is a pointer to the memory that will be copied into the
position at //offset//. If this is NULL, the position at //offset// will be
set to zero.

Returns the address of the item in the array, or NULL if the desired offset
is out of the array bounds.
*/
void* parray_rput( parray* array, size_t offset, void* item )
{
	PROC( "parray_rput" );
	PARMS( "array", "%p", array );
	PARMS( "offset", "%d", offset );
	PARMS( "item", "%p", item );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	RETURN( parray_put( array, array->last - offset, item ) );
}

/** Iterates over //array//.

Iterates over all items of //array// and calls the function //callback// on
every item. */
void parray_iter( parray* array, parrayfn callback )
{
	void*	ptr;

	PROC( "parray_iter" );
	PARMS( "array", "%p", array );
	PARMS( "callback", "%p", callback );

	if( !( array && callback ) )
	{
		WRONGPARAM;
		VOIDRET;
	}

	for( ptr = parray_first( array ); ptr; ptr = parray_next( array, ptr ) )
		(*callback)( ptr );

	VOIDRET;
}

/** Iterates backwards over //array//.

Backwardly iterates over all items of //array// and calls the function
//callback// on every item. */
void parray_riter( parray* array, parrayfn callback )
{
	void*	ptr;

	PROC( "parray_riter" );
	PARMS( "array", "%p", array );
	PARMS( "callback", "%p", callback );

	if( !( array && callback ) )
	{
		WRONGPARAM;
		VOIDRET;
	}

	for( ptr = parray_last( array ); ptr; ptr = parray_prev( array, ptr ) )
		(*callback)( ptr );

	VOIDRET;
}

/** Access first element of the array.

Returns the address of the accessed item, and (void*)NULL if nothing is in
the array.
*/
void* parray_first( parray* array )
{
	PROC( "parray_first" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->first == array->last )
		RETURN( (void*)NULL );

	RETURN( (char*)array->array + ( array->first * array->size ) );
}

/** Access last element of the array.

Returns the address of the accessed item, and (void*)NULL if nothing is in
the array.
*/
void* parray_last( parray* array )
{
	PROC( "parray_last" );
	PARMS( "array", "%p", array );

	if( !array )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	if( array->first == array->last )
		RETURN( (void*)NULL );

	RETURN( (char*)array->array + ( ( array->last - 1 ) * array->size ) );
}

/** Access next element from //ptr// in //array//.

Returns the address of the next element, and (void*)NULL if the access gets
out of bounds.
*/
void* parray_next( parray* array, void* ptr )
{
	PROC( "parray_next" );
	PARMS( "array", "%p", array );

	if( !( array && ptr ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	ptr += array->size;
	if( ptr > parray_last( array ) || ptr < parray_first( array ) )
		RETURN( (void*)NULL );

	RETURN( ptr );
}

/** Access previous element from //ptr// in //array//.

Returns the address of the previous element, and (void*)NULL if the access gets
out of bounds.
*/
void* parray_prev( parray* array, void* ptr )
{
	PROC( "parray_prev" );
	PARMS( "array", "%p", array );

	if( !( array && ptr ) )
	{
		WRONGPARAM;
		RETURN( (void*)NULL );
	}

	ptr -= array->size;
	if( ptr < parray_first( array ) || ptr > parray_last( array ) )
		RETURN( (void*)NULL );

	RETURN( ptr );
}

/** Swap two elements of an array. */
void* parray_swap( parray* array, size_t pos1, size_t pos2 )
{
	void*	ptr1;
	void*	ptr2;

	if( !( array ) )
	{
		WRONGPARAM;
		return (void*)NULL;
	}

	if( !( ( ptr1 = parray_get( array, pos1 ) )
			&& ( ptr2 = parray_get( array, pos2 ) ) ) )
		return (void*)NULL;

	if( ptr1 == ptr2 )
		return ptr1;

	parray_push( array, ptr1 );
	parray_put( array, pos1, ptr2 );
	parray_put( array, pos2, parray_pop( array ) );

	return ptr1;
}

/** Returns the number of elements in a array. */
size_t parray_count( parray* array )
{
	if( !array )
		return 0;

	return array->last - array->first;
}

/** Returns TRUE, if //ptr// is an element of array //array//. */
pboolean parray_partof( parray* array, void* ptr )
{
	if( !( array && ptr ) )
	{
		WRONGPARAM;
		return FALSE;
	}

	if( ptr >= parray_first( array ) && ptr <= parray_last( array ) )
		return TRUE;

	return FALSE;
}

/** Return offset of element //ptr// in array //array//.
Returns the offset of //ptr// in //array//.
The function returns the size of the array (which is an invalid offset)
if //ptr// is not part of //array//.

To check if a pointer belongs to an array, call parray_partof(). */
size_t parray_offset( parray* array, void* ptr )
{
	if( !( array && ptr ) )
	{
		WRONGPARAM;
		return 0;
	}

	if( !parray_partof( array, ptr ) )
		return parray_count( array );

	return ( (char*)ptr - ( (char*)array->array
								+ ( array->first * array->size ) ) )
				/ array->size;
}
