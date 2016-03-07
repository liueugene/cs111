/*
 * SortedList (and SortedListElement)
 *
 *	A doubly linked list, kept sorted by a specified key.
 *	This structure is used for a list head, and each element
 *	of the list begins with this structure.
 *
 *	The list head is in the list, and an empty list contains
 *	only a list head.  The list head is also recognizable because
 *	it has a NULL key pointer.
 */
// struct SortedListElement {
// 	struct SortedListElement *prev;
// 	struct SortedListElement *next;
// 	const char *key;
// };
// typedef struct SortedListElement SortedList_t;
// typedef struct SortedListElement SortedListElement_t;

//returns 0 if key should go after key2
//returns 1 if key should go before key2
//returns 2 if key and key2 are equivalent
//returns -1 if key2 is NULL;
int compare(const char* key, const char* key2) {
	int i = 0;

	while (key[i] != '\0' && key2[i] != '\0') {
		if (key[i] < key2[i]) {
			return 1;
		} else if (key[i] > key2[i]) {
			return 0;
		} else {
			i++;
		}
	}
	if (key[i] == '\0' && key2[i] == '\0') {
		return 2;
	} else if (key[i] == '\0') {
		return 1;
	} else {
		return 0;
	}
}


/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 *
 * Note: if (opt_yield & INSERT_YIELD)
 *		call pthread_yield in middle of critical section
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	SortedListElement_t* node = list->next;
	SortedListElement_t* prev_node = list;
	while (node->key != NULL) {
		if (compare(element->key, node->key)) {
			break;
		}
		prev_node = node;
		if (opt_yield & INSERT_YIELD) {
			pthread_yield();
		}
		node = node->next;
	}
	element->prev = prev_node;
	element->next = node;

	prev_node->next = element;
	if (opt_yield & INSERT_YIELD) {
		pthread_yield();
	}
	node->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 * Note: if (opt_yield & DELETE_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_delete(SortedListElement_t *element) {
	SortedListElement_t* prev_node = element->prev;
	SortedListElement_t* next_node = element->next;

	if (prev_node->next != element || next_node->prev != element) {
		return 1;
	}

	prev_node->next = next_node;
	if (opt_yield & DELETE_YIELD) {
		pthread_yield();
	}
	next_node->prev = prev_node;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	SortedListElement_t* temp_node = list;
	if (opt_yield & SEARCH_YIELD) {
		pthread_yield();
	}
	SortedListElement_t* node = temp_node->next;
	while (node->key != NULL) {
		if (compare(key, node->key) == 2) {
			return node;
		}
		temp_node = node;
		if (opt_yield & SEARCH_YIELD) {
			pthread_yield();
		}
		node = temp_node->next;
	}
	return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumerating list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_length(SortedList_t *list) {	
	SortedListElement_t* prev_node = list;
	if (opt_yield & SEARCH_YIELD){
		pthread_yield();
	}
	SortedListElement_t* node = prev_node->next;
	//SortedListElement_t* next_node = node->next;
	int count = 0;
	while (node->key != NULL) {
		if (prev_node->next != node && node->prev != prev_node) {
			return -1;
		}
		count++;
		prev_node = node;
		if (opt_yield & SEARCH_YIELD){
			pthread_yield();
		}
		node = prev_node->next;
		//next_node = node->next;
	}
	if (prev_node->next != node && node->prev != prev_node) {
		return -1;
	}
	return count;
}

/**
 * variable to enable diagnositc calls to pthread_yield
 */
// extern int opt_yield;
// #define	INSERT_YIELD	0x01	// yield in insert critical section
// #define	DELETE_YIELD	0x02	// yield in delete critical section
// #define	SEARCH_YIELD	0x04	// yield in lookup/length critical section