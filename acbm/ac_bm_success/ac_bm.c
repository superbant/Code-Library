#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ac_bm.h"
#include "v_optimize.h"


//#define CASE_SENSITIVE
//#define DEBUG_SEARCH

static int ACtree_build(pattern_tree *ptree, \
		pattern_data *patterns, \
		int npattern);
static int ACtree_compute_BCshifts(pattern_tree *ptree);
static int set_GSshift(pattern_tree *ptree, unsigned char *pat, int depth, int shift);
static int compute_GSshift(pattern_tree *ptree, unsigned char *pat1, \
		int pat1_len, unsigned char *pat2, int pat2_len);
static int ACtree_compute_shifts(pattern_tree *ptree);
static int ACtree_compute_GSshifts(pattern_tree *ptree);
static int _init_GSshifts(pattern_tree_node *root, int shift);
static int ACtree_init_GSshifts(pattern_tree *ptree);
//static void ACtree_print_tree(pattern_tree *ptree);
static void _print_tree(pattern_tree_node *root);
static void _clean_tree(pattern_tree_node *root);
/*added by wyang*/
static int ACtreeback_build(pattern_tree *ptree, \
		pattern_data *patterns, \
		int npattern);
//
// ACtree_build
//
static int ACtree_build(pattern_tree *ptree, \
		pattern_data *patterns, \
		int npattern)
{
	int i;
	pattern_tree_node *root = NULL, *parent = NULL;
	unsigned char ch;
	int max_pattern_len = 0, min_pattern_len = PATTERN_LEN;

	if (ptree == NULL || patterns == NULL || npattern < 0)
		goto err;
	
	// create root tree node
	root = malloc(sizeof(pattern_tree_node));
	if (root == NULL)
		goto err;
	bzero(root, sizeof(pattern_tree_node));
	root->label = -2;
	root->depth = 0;
	
	// add pattern node to tree
	for (i = 0; i < npattern; i++) 
	{
		int pat_len;
		int ch_i;
		
		pat_len = (patterns+i)->len;
		if (pat_len == 0)
			continue;
		else if (pat_len > PATTERN_LEN)
			pat_len = PATTERN_LEN;
		
		
		if (pat_len > max_pattern_len)
			max_pattern_len = pat_len;
		if (pat_len < min_pattern_len)
			min_pattern_len = pat_len;

		/* search branch adding point */
		parent = root;
		for (ch_i = 0; ch_i < pat_len; ch_i++)
		{
			ch = ((patterns+i)->data)[ch_i]; 
			if (!(patterns+i)->case_sen) {
				ch = tolower(ch);
			}

			if (parent->childs[ch] == NULL) { /* find adding point */
				break;
			}
			
			parent = parent->childs[ch];
		}
		
		if (ch_i < pat_len) {
			/* add the branch under 'parent' */
			for (; ch_i < pat_len; ch_i++)
			{
				pattern_tree_node *node = NULL;

				ch = ((patterns+i)->data)[ch_i];
				if (!(patterns+i)->case_sen) {
					ch = tolower(ch);
				}
				
				/* alloc node */
				node = malloc(sizeof(pattern_tree_node));
				if (node == NULL)
					goto err;
				bzero(node, sizeof(pattern_tree_node));
				node->depth = ch_i + 1;  // from 1
				node->ch = ch;
				node->case_sen = (patterns+i)->case_sen;
				node->label = -1;
				
				/* add to tree */
				parent->childs[ch] = node;
				/* no case sensitive */
				if (!(patterns+i)->case_sen) {
					if (ch >= 'a' && ch <= 'z') {
						parent->childs[ch-32] = node;
					}
				}	

				parent->nchild++;
				parent->one_child = ch;
				node->parent = parent;
				
				parent = node;
			}
		}

		/* last node remember which pattern where it from */
		parent->label = i;
	}
	
	ptree->pattern_list = patterns;
	ptree->pattern_count = npattern;
	ptree->root = root;
	ptree->max_depth = max_pattern_len;
	ptree->min_pattern_size = min_pattern_len;

	return 0;
err:
	/* free tree */
	if (ptree && ptree->root != NULL)
	{
		_clean_tree(ptree->root);
		free(ptree->root);
		ptree->root = NULL;
	}
	return -1;
}


/*
 * _print_tree
 */
static void _print_tree(pattern_tree_node *root)
{
	int i;

	if (root == NULL)
		return;
	// print this node
	printf("ch:%c shift:%d label:%d depth:%d childs:", root->ch, root->GSshift, root->label, root->depth);
	
	for (i = 0; i < 256; i++) {
		if (!root->case_sen) {
			if (i >= 'A' && i <= 'Z') {
				continue;
			}
		}
		if (root->childs[i])
			printf("%c ", (root->childs[i])->ch);
	}
	printf("\n");

	// print child tree recursively
	for (i = 0; i < 256; i++) {
		if (root->childs[i])
			_print_tree(root->childs[i]);
	}

	return;
}

#if 0 /* unused */
/*
 * ACtree_print_tree
 */
static void ACtree_print_tree(pattern_tree *ptree)
{
	if (ptree == NULL)
		return;

	if (ptree->root)
		_print_tree(ptree->root);

	return;
}
#endif 

/*
 * ACtree_compute_BCshifts
 */
static int ACtree_compute_BCshifts(pattern_tree *ptree)
{
	int i, j = 0;

	for (i = 0; i < 256; i++)
		ptree->BCshift[i] = ptree->min_pattern_size;

	for (i = ptree->min_pattern_size - 1; i > 0; i--)
		for (j = 0; j < ptree->pattern_count; j++) {
			unsigned char ch;

			ch = (ptree->pattern_list+j)->data[i];
			if (!ptree->pattern_list[0].case_sen) {
				ch = tolower(ch);
			}
			ptree->BCshift[ch] = i;
			if (!ptree->pattern_list[0].case_sen) {
				if (ch >= 'a' && ch <='z') {
					ptree->BCshift[ch-32] = i;
				}
			}
		}

	return 0;
}

/*
 * set_GSshift
 */
static int set_GSshift(pattern_tree *ptree, unsigned char *pat, int depth, int shift)
{
	int i;
	pattern_tree_node *node;

	if (ptree == NULL || ptree->root == NULL)
		goto err;

	node = ptree->root;
	for (i = 0; i < depth; i++) {
		node = node->childs[pat[i]];
		if (node == NULL)
			goto err;
	}
	
	node->GSshift = node->GSshift < shift ? node->GSshift : shift;

	return 0;
err:
	return -1;
}

/*
 * compute_GSshift
 */
static int compute_GSshift(pattern_tree *ptree, unsigned char *pat1, \
		int pat1_len, unsigned char *pat2, int pat2_len)
{
	unsigned char first_char;
	int i;
	int pat1_index, pat2_index, offset;


	if (pat1 == NULL || pat2 == NULL || pat1_len < 0 || pat2_len < 0)
		goto err;

	if (pat1_len == 0 || pat2_len == 0)
		return 0;

	first_char = pat1[0];
	if (!ptree->pattern_list[0].case_sen) {
		first_char = tolower(first_char);
	}
	/* handle char 0 separately */
	for (i = 1; i < pat2_len; i++) {
		if (!ptree->pattern_list[0].case_sen) {
			if (tolower(pat2[i]) != first_char) {
				break;
			}
		} else {
			if (pat2[i] != first_char) {
				break;
			}
		}
	}
	set_GSshift(ptree, pat1, 1, i);

	i = 1;
	while (1) {
		// search first char in pat2
		if (!ptree->pattern_list[0].case_sen) {
			while (i < pat2_len && tolower(pat2[i]) != first_char) {
				i++;
			}
		} else {
			while (i < pat2_len && pat2[i] != first_char) {
				i++;
			}
		}
		
		if (i == pat2_len) // not found the first char
			break;
		
		pat2_index = i;
		pat1_index = 0;
		offset = i;
		
		/* optimization: >min_pattern_size, not check*/
		if (offset > ptree->min_pattern_size)
			break;
		
		while (pat2_index < pat2_len && pat1_index < pat1_len)
		{
			if (!ptree->pattern_list[0].case_sen) {
				if (tolower(pat1[pat1_index]) != tolower(pat2[pat2_index])) { /*found different*/
					break;
				}
			} else {
				if (pat1[pat1_index] != pat2[pat2_index]) { /*found different*/
					break;
				}
			}
			pat1_index++;
			pat2_index++;
		}
		
		if (pat1_index == pat1_len) {
			/* pat2 contained pat1 ??? how to do */
		} else if (pat2_index == pat2_len) {
			int j;
			/* 1.pat2 contained some part of pat1 */
			for (j = pat1_index; j < pat1_len; j++) {
				set_GSshift(ptree, pat1, j+1, offset);
			}
		} else /* 2.found one substring in pat2 */ 
			set_GSshift(ptree, pat1, pat1_index+1, offset);
		
		i++;
	}

	return 0;
	
err:
	return -1;
}

/*
 * ACtree_compute_GSshifts
 */
static int ACtree_compute_GSshifts(pattern_tree *ptree)
{
	int pat_i = 0, pat_j = 0;
	
	for (pat_i = 0; pat_i < ptree->pattern_count; pat_i++)
	{
		for (pat_j = 0; pat_j < ptree->pattern_count; pat_j++) {
			unsigned char *ppat_i = (ptree->pattern_list+pat_i)->data;
			int patlen_i = (ptree->pattern_list+pat_i)->len;
			unsigned char *ppat_j = (ptree->pattern_list+pat_j)->data;
			int patlen_j = (ptree->pattern_list+pat_j)->len;

			compute_GSshift(ptree, ppat_i, patlen_i, ppat_j, patlen_j);
		}
	}

	return 0;
}

/*
 * _init_GSshifts
 */
static int _init_GSshifts(pattern_tree_node *root, int shift)
{
	int i;

	if (root->label != -2)
		root->GSshift = shift;
			
	for (i = 0; i < 256; i++)
	{
		if (!root->case_sen) {
			if (i >= 'A' && i <= 'Z') {
				continue;
			}
		}

		if (root->childs[i] != NULL) {
			_init_GSshifts(root->childs[i], shift);
		}
	}

	return 0;
}

/*
 * ACtree_init_GSshifts
 */
static int ACtree_init_GSshifts(pattern_tree *ptree)
{
	_init_GSshifts(ptree->root, ptree->min_pattern_size);

	return 0;
}

/*
 * ACtree_compute_shifts
 */
static int ACtree_compute_shifts(pattern_tree *ptree)
{
	ACtree_compute_BCshifts(ptree);
	
	ACtree_init_GSshifts(ptree);
	ACtree_compute_GSshifts(ptree);

	return 0;
}


pattern_tree *acbm_init(pattern_data *patterns, int npattern)
{
	pattern_tree *ptree = NULL;
	
	ptree = (pattern_tree *)malloc(sizeof(pattern_tree));
	if (!ptree)
		goto err;

	bzero(ptree, sizeof(pattern_tree));

	ACtree_build(ptree, patterns, npattern);
	
	ACtree_compute_shifts(ptree);

	//ACtree_print_tree(ptree);

	return ptree;

err:
	return NULL;
}

/*
 * acbm_search
 * return value:
 * n: matched n patterns
 * 0: not matched
 * <0: error
 */
inline int acbm_search(pattern_tree *ptree, unsigned char *text, int text_len, \
		unsigned int matched_indexs[], int nmax_index)
{
	int nmatched = 0;
	register int base_index = 0, cur_index = 0;
	register int real_shift = 0, gs_shift = 0, bc_shift = 0;
	pattern_tree_node *node = NULL;
	
	/*
	if (ptree == NULL || text == NULL || text_len < 0)
		goto err;
	
	if (ptree->root == NULL)
		goto err;
	
	*/
	if (text_len < ptree->min_pattern_size)
		goto ret;
	
	base_index = text_len - ptree->min_pattern_size;
	
	while (base_index >= 0)
	{
		cur_index = base_index;

		node = ptree->root;
		
#ifdef DEBUG_SEARCH
		printf("Checking pattern tree at %d...", base_index);
#endif
		while (node->childs[text[cur_index]] != NULL)
		{
			node = node->childs[text[cur_index]];
			
			if (unlikely(NULL == node)) {
				goto ret;
			}

			if (node->label >= 0) {
				/* matched a pattern! */
#ifdef DEBUG_SEARCH
				printf("Matched(%d) ", node->label);
#endif
				matched_indexs[nmatched] = node->label;
				nmatched++;
				if (nmatched == nmax_index)
					goto ret;
			}

			cur_index++;

			if (cur_index >= text_len)
				break;
		}
		
		if (node->nchild > 0) {
			/* match fail */
			gs_shift = node->childs[node->one_child]->GSshift;
			if (cur_index < text_len)
				bc_shift = ptree->BCshift[text[cur_index]]+base_index-cur_index;
			else
				bc_shift = 1;
			real_shift = gs_shift > bc_shift ? gs_shift : bc_shift;
			base_index -= real_shift;
#ifdef DEBUG_SEARCH
			printf("Failed, GSshift:%d, BCshift:%d Realshift%d", gs_shift, bc_shift, real_shift);
#endif
		} else {
			/* match successful, ??? to be continued */
			base_index--;
#ifdef DEBUG_SEARCH
			printf("Matched, shift %d", 1);
#endif
		}
#ifdef DEBUG_SEARCH
		printf("\n");
#endif
	}
	
ret:
	return nmatched;
}

static void _clean_tree(pattern_tree_node *root)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		if (!root->case_sen) {
			if (i >= 'A' && i <= 'Z') {
				continue;
			}
		}
		if (root->childs[i]) {
			_clean_tree(root->childs[i]);

			free(root->childs[i]);
			root->childs[i] = NULL;
		}
	}

	return;
}

/*
 * acbm_clean
 */
void acbm_clean(pattern_tree *ptree)
{
	if (ptree == NULL)
		return;

	if (ptree->root) 
	{
		_clean_tree(ptree->root);
		free(ptree->root);
		ptree->root = NULL;
	}

	free(ptree);

//	ACtree_print_tree(ptree);
	return;
}


inline int acbm_search_ex(pattern_tree *ptree, unsigned char *text, int text_len, \
		matched_info_t matched_items[], int nmax_index)
{
	int nmatched = 0;
	register int base_index = 0, cur_index = 0;
	register int real_shift = 0, gs_shift = 0, bc_shift = 0;
	pattern_tree_node *node = NULL;
	
	/*
	if (ptree == NULL || text == NULL || text_len < 0)
		goto err;
	
	if (ptree->root == NULL)
		goto err;
	
	*/
	if (text_len < ptree->min_pattern_size)
		goto ret;
	
	base_index = text_len - ptree->min_pattern_size;
	
	while (base_index >= 0)
	{
		cur_index = base_index;

		node = ptree->root;
		
#ifdef DEBUG_SEARCH
		printf("Checking pattern tree at %d...", base_index);
#endif
		while (node->childs[text[cur_index]] != NULL) {
			node = node->childs[text[cur_index]];
			
			if (node->label >= 0) {
				/* matched a pattern! */
#ifdef DEBUG_SEARCH
				printf("Matched(%d) ", node->label);
#endif
				matched_items[nmatched].pattern_i = node->label;
				matched_items[nmatched].offset = base_index; /* postion */
#ifdef DEBUG_SEARCH
				printf("%s\n", text + matched_items[nmatched].offset);
#endif
				nmatched++;


				if (nmatched == nmax_index)
					goto ret;
			}

			cur_index++;

			if (cur_index >= text_len)
				break;
		}
		
		if (node->nchild > 0) {
			/* match fail */
			gs_shift = node->childs[node->one_child]->GSshift;
			if (cur_index < text_len)
				bc_shift = ptree->BCshift[text[cur_index]]+base_index-cur_index;
			else
				bc_shift = 1;
			real_shift = gs_shift > bc_shift ? gs_shift : bc_shift;
			base_index -= real_shift;
#ifdef DEBUG_SEARCH
			printf("Failed, GSshift:%d, BCshift:%d Realshift%d", gs_shift, bc_shift, real_shift);
#endif
		} else {
			/* match successful, ??? to be continued */
			base_index--;
#ifdef DEBUG_SEARCH
			printf("Matched, shift %d", 1);
#endif
		}
#ifdef DEBUG_SEARCH
		printf("\n");
#endif
	}
	
ret:
	return nmatched;
}

/*added by wyang*/
static int ACtreeback_build(pattern_tree *ptree, \
		pattern_data *patterns, \
		int npattern)
{
	int i;
	pattern_tree_node *root = NULL, *parent = NULL;
	unsigned char ch;
	int max_pattern_len = 0, min_pattern_len = PATTERN_LEN;

	if (ptree == NULL || patterns == NULL || npattern < 0)
		goto err;
	
	// create root tree node
	root = malloc(sizeof(pattern_tree_node));
	if (root == NULL)
		goto err;
	bzero(root, sizeof(pattern_tree_node));
	root->label = -2;
	root->depth = 0;
	
	// add pattern node to tree
	for (i = 0; i < npattern; i++) 
	{
		int pat_len;
		int ch_i;
		
		pat_len = (patterns+i)->len;
		if (pat_len == 0)
			continue;
		else if (pat_len > PATTERN_LEN)
			pat_len = PATTERN_LEN;
		
		
		if (pat_len > max_pattern_len)
			max_pattern_len = pat_len;
		if (pat_len < min_pattern_len)
			min_pattern_len = pat_len;

		/* search branch adding point */
		parent = root;
		for (ch_i = pat_len - 1; ch_i >= 0; ch_i--)
		{
			ch = ((patterns+i)->data)[ch_i]; 
			if (!(patterns+i)->case_sen) {
				ch = tolower(ch);
			}

			if (parent->childs[ch] == NULL) { /* find adding point */
				break;
			}
			
			parent = parent->childs[ch];
		}
		
		if (ch_i >= 0) {
			/* add the branch under 'parent' */
			for (; ch_i >= 0; ch_i--)
			{
				pattern_tree_node *node = NULL;

				ch = ((patterns+i)->data)[ch_i];
				if (!(patterns+i)->case_sen) {
					ch = tolower(ch);
				}
				
				/* alloc node */
				node = malloc(sizeof(pattern_tree_node));
				if (node == NULL)
					goto err;
				bzero(node, sizeof(pattern_tree_node));
				node->depth = ch_i + 1;  // from 1
				node->ch = ch;
				node->label = -1;
				
				/* add to tree */
				parent->childs[ch] = node;
				/* no case sensitive */
				if (!(patterns+i)->case_sen) {
					if (ch >= 'a' && ch <= 'z') {
						parent->childs[ch-32] = node;
					}
				}	

				parent->nchild++;
				parent->one_child = ch;
				node->parent = parent;
				
				parent = node;
			}
		}

		/* last node remember which pattern where it from */
		parent->label = i;
	}
	
	ptree->pattern_list = patterns;
	ptree->pattern_count = npattern;
	ptree->root = root;
	ptree->max_depth = max_pattern_len;
	ptree->min_pattern_size = min_pattern_len;

	return 0;
err:
	/* free tree */
	if (ptree && ptree->root != NULL)
	{
		_clean_tree(ptree->root);
		free(ptree->root);
		ptree->root = NULL;
	}
	return -1;
}

/*added by wyang*/
pattern_tree *acbmback_init(pattern_data *patterns, int npattern)
{
	pattern_tree *ptree = NULL;
	
	ptree = (pattern_tree *)malloc(sizeof(pattern_tree));
	if (!ptree)
		goto err;

	bzero(ptree, sizeof(pattern_tree));

	ACtreeback_build(ptree, patterns, npattern);
	
//	ACtree_print_tree(ptree);

	return ptree;

err:
	return NULL;
}
