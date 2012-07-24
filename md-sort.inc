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
	
	return 0;
}

#define SortArrayUsingComparator(%1,%2) \
	SortArrayUsingComparator_Entry(sizeof(%1), %1, !"\0\0\0\0" #%2)

#define Comparator:%1(%2) \
	forward %1(%2); public %1(%2)

enum e_COMPARE_ORDER {
	ORDER_ASCENDING = -1,
	ORDER_EQUAL,
	ORDER_DESCENDING
};

static stock GetFunctionAddress(const funcname[]) {
	new idx, address;
	
	if (-1 != (idx = funcidx(funcname))) {
		#emit LCTRL		   1
		#emit NEG
		#emit MOVE.alt
		#emit ADD.C		   32
		#emit STOR.S.pri   address
		#emit LREF.S.pri   address
		#emit ADD
		#emit LOAD.S.alt   idx
		#emit SHL.C.alt	   3
		#emit ADD
		#emit STOR.S.pri   address
		#emit LREF.S.pri   address
		#emit STOR.S.pri   address
	}
	
	return address;
}

stock SortArrayUsingComparator_Entry(size, ...) {
	new array, func;
	
	#emit LOAD.S.pri  16
	#emit STOR.S.pri  array
	#emit LOAD.S.pri  20
	#emit LOAD.I
	#emit STOR.S.pri  func
	
	if (!func) {
		#emit LOAD.S.pri  20
		#emit ADD.C       4
		#emit PUSH.pri
		#emit PUSH.C      4
		#emit LCTRL       6
		#emit ADD.C       28
		#emit PUSH.pri
		#emit CONST.pri   GetFunctionAddress
		#emit SCTRL       6
		#emit LOAD.S.alt  20
		#emit STOR.I
		#emit STOR.S.pri  func
	}
	
	if (!func) {
		print(!"(SortArrayUsingComparator) Invalid comparator given.");
		
		return;
	}
	
	SortArrayUsingComparator_QS(array, func, 0, size - 1);
}

stock SortArrayUsingComparator_QS(array, func, left, right) {
	new
		left_hold = left,
		right_hold = right,
		pivot = left
	;
	
	while (left_hold <= right_hold) {
		new result;
		
		for (;;) {
			#emit LOAD.S.pri  array
			#emit LOAD.S.alt  pivot
			#emit SHL.C.alt   2
			#emit ADD
			#emit PUSH.pri
			#emit LOAD.I
			#emit POP.alt
			#emit ADD
			#emit PUSH.pri
			
			#emit LOAD.S.pri  array
			#emit LOAD.S.alt  left_hold
			#emit SHL.C.alt   2
			#emit ADD
			#emit PUSH.pri
			#emit LOAD.I
			#emit POP.alt
			#emit ADD
			#emit PUSH.pri
			
			#emit PUSH.C      8
			#emit LCTRL       6
			#emit ADD.C       28
			#emit PUSH.pri
			#emit LOAD.S.pri  func
			#emit SCTRL       6
			#emit STOR.S.pri  result
			
			if (result < 0) {
				left_hold++;
			} else {
				break;
			}
		}
		
		for (;;) {
			#emit LOAD.S.pri  array
			#emit LOAD.S.alt  pivot
			#emit SHL.C.alt   2
			#emit ADD
			#emit PUSH.pri
			#emit LOAD.I
			#emit POP.alt
			#emit ADD
			#emit PUSH.pri
			
			#emit LOAD.S.pri  array
			#emit LOAD.S.alt  right_hold
			#emit SHL.C.alt   2
			#emit ADD
			#emit PUSH.pri
			#emit LOAD.I
			#emit POP.alt
			#emit ADD
			#emit PUSH.pri
			
			#emit PUSH.C      8
			#emit LCTRL       6
			#emit ADD.C       28
			#emit PUSH.pri
			#emit LOAD.S.pri  func
			#emit SCTRL       6
			#emit STOR.S.pri  result
			
			if (result > 0) {
				right_hold--;
			} else {
				break;
			}
		}
		
		if (left_hold <= right_hold) {
			#emit PUSH.S      right_hold
			#emit PUSH.S      left_hold
			#emit PUSH.S      array
			#emit PUSH.C      12
			#emit LCTRL       6
			#emit ADD.C       28
			#emit PUSH.pri
			#emit CONST.pri   ExchangeArraySlots
			#emit SCTRL       6

			left_hold++, right_hold--;
		}
	}
	
	if (left < right_hold) SortArrayUsingComparator_QS(array, func, left, right_hold);
	if (left_hold < right) SortArrayUsingComparator_QS(array, func, left_hold, right);
}

#define SortArrayUsingComparator_Entry(%1)%2=>%3; \
	SortArrayUsingCompInto_Entry(%3, %1);

enum (<<= 1) {
	SORT_IS_PLAYERS = 1
};

stock SortArrayUsingCompInto_Entry(results[], size, ...) {
	new array, func, flags;
	
	#emit LOAD.S.pri  20
	#emit STOR.S.pri  array
	#emit LOAD.S.pri  24
	#emit LOAD.I
	#emit STOR.S.pri  func
	
	if (numargs() > 4)
		flags = getarg(4);
	
	if (!func) {
		#emit LOAD.S.pri  24
		#emit ADD.C       4
		#emit PUSH.pri
		#emit PUSH.C      4
		#emit LCTRL       6
		#emit ADD.C       28
		#emit PUSH.pri
		#emit CONST.pri   GetFunctionAddress
		#emit SCTRL       6
		#emit LOAD.S.alt  24
		#emit STOR.I
		#emit STOR.S.pri  func
	}
	
	if (!func) {
		print(!"(SortArrayUsingComparator) Invalid comparator given.");
		
		return;
	}
	
	if (flags & SORT_IS_PLAYERS) {
		new connected = 0;
		
		// Start filling the result array with connected players
		for (new i = 0; i < size; i++) {
			if (IsPlayerConnected(i))
				results[connected++] = i;
		}
		
		// Fill the remaining slots with INVALID_PLAYER_ID
		for (new i = connected; i < size; i++)
			results[i] = -1;
		
		// Store the original size in the flags
		flags |= size << 16;
		
		// Set the size so only the connected players will be sorted
		size = connected;
	} else {
		for (new i = 0; i < size; i++)
			results[i] = i;
	}
	
	SortArrayUsingCompInto_QS(array, results, func, 0, size - 1);
	
	if (flags & SORT_IS_PLAYERS) {
		if (size < flags >>> 16 && results[size] == -1)
			results[size] = INVALID_PLAYER_ID;
	}
}

stock SortArrayUsingCompInto_QS(array, results[], func, left, right) {
	new
		left_hold = left,
		right_hold = right,
		pivot = left
	;
	
	while (left_hold <= right_hold) {
		new result;
		
		for (;;) {
			if (results[pivot] == -1 && results[left_hold] == -1) {
				result = 0;
			} else if (results[pivot] == -1) {
				result = -1;
			} else if (results[left_hold] == -1) {
				result = 1;
			} else {
				// Load results[pivot]
				#emit LOAD.S.pri  pivot
				#emit SHL.C.pri   2  
				#emit LOAD.S.alt  results
				#emit ADD
				#emit LOAD.I
				#emit MOVE.alt
				
				// Push array[results[pivot]]
				#emit LOAD.S.pri  array
				#emit SHL.C.alt   2
				#emit ADD
				#emit PUSH.pri
				#emit LOAD.I
				#emit POP.alt
				#emit ADD
				#emit PUSH.pri
				
				// Load results[left_hold]
				#emit LOAD.S.pri  left_hold
				#emit SHL.C.pri   2  
				#emit LOAD.S.alt  results
				#emit ADD
				#emit LOAD.I
				#emit MOVE.alt
				
				// Push array[results[left_hold]]
				#emit LOAD.S.pri  array
				#emit SHL.C.alt   2
				#emit ADD
				#emit PUSH.pri
				#emit LOAD.I
				#emit POP.alt
				#emit ADD
				#emit PUSH.pri
				
				// Push arg. count
				#emit PUSH.C      8
				// Push return address
				#emit LCTRL       6
				#emit ADD.C       28
				#emit PUSH.pri
				// Call the comparator
				#emit LOAD.S.pri  func
				#emit SCTRL       6
				// Store the return
				#emit STOR.S.pri  result
			}
			
			if (result < 0) {
				left_hold++;
			} else {
				break;
			}
		}
		
		for (;;) {
			if (results[pivot] == -1 && results[right_hold] == -1) {
				result = 0;
			} else if (results[pivot] == -1) {
				result = -1;
			} else if (results[right_hold] == -1) {
				result = 1;
			} else {
				// Load results[pivot]
				#emit LOAD.S.pri  pivot
				#emit SHL.C.pri   2  
				#emit LOAD.S.alt  results
				#emit ADD
				#emit LOAD.I
				#emit MOVE.alt
				
				// Push array[results[pivot]]
				#emit LOAD.S.pri  array
				#emit SHL.C.alt   2
				#emit ADD
				#emit PUSH.pri
				#emit LOAD.I
				#emit POP.alt
				#emit ADD
				#emit PUSH.pri
			
				// Load results[right_hold]
				#emit LOAD.S.pri  right_hold
				#emit SHL.C.pri   2  
				#emit LOAD.S.alt  results
				#emit ADD
				#emit LOAD.I
				#emit MOVE.alt
			
				// Push array[results[right_hold]]
				#emit LOAD.S.pri  array
				#emit SHL.C.alt   2
				#emit ADD
				#emit PUSH.pri
				#emit LOAD.I
				#emit POP.alt
				#emit ADD
				#emit PUSH.pri
			
				// Push arg. count
				#emit PUSH.C      8
				// Push return address
				#emit LCTRL       6
				#emit ADD.C       28
				#emit PUSH.pri
				// Call the comparator
				#emit LOAD.S.pri  func
				#emit SCTRL       6
				// Store the return
				#emit STOR.S.pri  result
			}
			
			if (result > 0) {
				right_hold--;
			} else {
				break;
			}
		}
		
		if (left_hold <= right_hold) {
			// used instead of creating another variable
			result = results[right_hold];
			
			results[right_hold] = results[left_hold];
			results[left_hold] = result;
			
			left_hold++, right_hold--;
		}
	}
	
	if (left < right_hold) SortArrayUsingCompInto_QS(array, results, func, left, right_hold);
	if (left_hold < right) SortArrayUsingCompInto_QS(array, results, func, left_hold, right);
}