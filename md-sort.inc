// md-sort.inc by Slice

#include <a_samp>

#define SortDeepArray(%1,%2) SortDeepArray_Entry(%1, sizeof(%1), %1[0][%2])

// Use global variables to be nicer to the stack
stock
	g_sort_cmp_offset,
	g_sort_cmp_type
;

stock SortDeepArray_Entry(array[][], size, &{Float, String, string, _}:cmp, cmp_tag = tagof(cmp)) {
	if (cmp_tag == tagof(Float:))
		g_sort_cmp_type = 'f';
	else
		g_sort_cmp_type = 'i';
	
	// Get the offset between array[0][0] and array[0][cmp]
	#emit LOAD.S.pri array
	#emit MOVE.alt
	#emit LOAD.I
	#emit ADD
	#emit MOVE.alt
	#emit LOAD.S.pri cmp
	#emit SUB
	#emit SHR.C.pri  2
	#emit STOR.pri   g_sort_cmp_offset
	
	_SortDeepArray(array, 0, size -  1);
}

stock _SortDeepArray(array[][], left, right) {
	new
		left_hold = left,
		right_hold = right,
		pivot = array[(left + right) / 2][g_sort_cmp_offset]
	;
	
	while (left_hold <= right_hold) {
		switch (g_sort_cmp_type) {
			case 'f': {
				while (Float:array[left_hold][g_sort_cmp_offset] < Float:pivot) left_hold++;
				while (Float:array[right_hold][g_sort_cmp_offset] > Float:pivot) right_hold--;
			}
			
			default: {
				while (array[left_hold][g_sort_cmp_offset] < pivot) left_hold++;
				while (array[right_hold][g_sort_cmp_offset] > pivot) right_hold--;
			}
		}
		
		if (left_hold <= right_hold) {
			ExchangeArraySlots(array, left_hold, right_hold);
			
			left_hold++, right_hold--;
		}
	}
	
	if (left < right_hold) _SortDeepArray(array, left, right_hold);
	if (left_hold < right) _SortDeepArray(array, left_hold, right);
}

stock ExchangeArraySlots(array[][], slot1, slot2) {
	new
		addr1,
		addr2;
	
	// Get the pointer and its address  for slot1
	#emit LOAD.S.pri  array
	#emit LOAD.S.alt  slot1
	#emit SHL.C.alt   2
	#emit ADD
	#emit MOVE.alt
	
	#emit STOR.S.pri  slot1
	#emit LOAD.I
	#emit ADD
	#emit STOR.S.pri  addr1
	
	// Get the pointer and its address for slot2
	#emit LOAD.S.pri  array
	#emit LOAD.S.alt  slot2
	#emit SHL.C.alt   2
	#emit ADD
	#emit MOVE.alt
	
	#emit STOR.S.pri  slot2
	#emit LOAD.I
	#emit ADD
	#emit STOR.S.pri  addr2
	
	// Swap them
	#emit LOAD.S.pri  addr2
	#emit LOAD.S.alt  slot1
	#emit SUB
	#emit STOR.I
	
	#emit LOAD.S.pri  addr1
	#emit LOAD.S.alt  slot2
	#emit SUB
	#emit STOR.I
}