// md-sort.inc by Slice
//
// SortDeepArray(array[][], sort_index, .order = SORT_ASC, .ignorecase = false)

#include <a_samp>

#define SortDeepArray(%1) (_:SA_0:SA_1:SA_2:SA_3:SortDeepArray_Entry(%1)) 
#define SA_0:SA_1:SA_2:SA_3:SortDeepArray_Entry(%1,%2,%3) SA_2:SA_3:SortDeepArray_Entry(%1, sizeof(%1), %1[0][%2], %1[0][(%2) - (%2)], %3)
#define SA_1:SA_2:SA_3:SortDeepArray_Entry(%1,%2) SA_2:SA_3:SortDeepArray_Entry(%1, sizeof(%1), %1[0][%2], %1[0][(%2) - (%2)], _)
#define SA_2:SA_3:SortDeepArray_Entry(%1,%2,%3[0][%4string%5:%6],%7,%8) SortDeepArray_Entry(%1,%2,%3[0][%6],%3[0][(%6) - (%6)],%8,.cmp_string=true)

enum E_SORT_ORDER {
	SORT_ASC,
	SORT_DESC
};

// Use global variables to be nicer to the stack
stock
	             g_sort_cmp_offset,
	             g_sort_cmp_type,
	E_SORT_ORDER:g_sort_order,
	        bool:g_sort_ignorecase
;

stock SortDeepArray_Entry(array[][], size, &{Float, String, string, _}:cmp1, &{Float, String, string, _}:cmp2, cmp_tag = tagof(cmp1), bool:cmp_string = false, bool:ignorecase = false, E_SORT_ORDER:order = SORT_ASC) {
	if (cmp_string)
		g_sort_cmp_type = 's';
	else if (cmp_tag == tagof(Float:))
		g_sort_cmp_type = 'f';
	else
		g_sort_cmp_type = 'i';
	
	g_sort_ignorecase = ignorecase;
	g_sort_order = order;
	
	#emit LOAD.S.pri  cmp1
	#emit LOAD.S.alt  cmp2
	#emit SUB
	#emit SHR.C.pri   2
	#emit STOR.pri    g_sort_cmp_offset
	
	_SortDeepArray(array, 0, size -  1);
}

stock _SortDeepArray(array[][], left, right) {
	new
		left_hold = left,
		right_hold = right,
		pivot_idx = (left + right) / 2,
		pivot = array[pivot_idx][g_sort_cmp_offset]
	;
	
	while (left_hold <= right_hold) {
		switch (g_sort_cmp_type) {
			case 'f': {
				if (g_sort_order == SORT_ASC) {
					while (Float:array[left_hold][g_sort_cmp_offset] < Float:pivot) left_hold++;
					while (Float:array[right_hold][g_sort_cmp_offset] > Float:pivot) right_hold--;
				} else {
					while (Float:array[left_hold][g_sort_cmp_offset] > Float:pivot) left_hold++;
					while (Float:array[right_hold][g_sort_cmp_offset] < Float:pivot) right_hold--;
				}
			}
			
			case 's': {
				if (g_sort_order == SORT_ASC) {
					while (strcmp(array[left_hold][g_sort_cmp_offset], array[pivot_idx][g_sort_cmp_offset], g_sort_ignorecase) < 0) left_hold++;
					while (strcmp(array[right_hold][g_sort_cmp_offset], array[pivot_idx][g_sort_cmp_offset], g_sort_ignorecase) > 0) right_hold--;
				} else {
					while (strcmp(array[left_hold][g_sort_cmp_offset], array[pivot_idx][g_sort_cmp_offset], g_sort_ignorecase) > 0) left_hold++;
					while (strcmp(array[right_hold][g_sort_cmp_offset], array[pivot_idx][g_sort_cmp_offset], g_sort_ignorecase) < 0) right_hold--;
				}
			}
			
			default: {
				if (g_sort_order == SORT_ASC) {
					while (array[left_hold][g_sort_cmp_offset] < pivot) left_hold++;
					while (array[right_hold][g_sort_cmp_offset] > pivot) right_hold--;
				} else {
					while (array[left_hold][g_sort_cmp_offset] > pivot) left_hold++;
					while (array[right_hold][g_sort_cmp_offset] < pivot) right_hold--;
				}
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