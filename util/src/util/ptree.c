#include "util/ptree.h"
#include "util/memory.h"
#include "util/string.h"
#include <string.h>
#include <assert.h>

static const char node_delim = '.';

/* ------------------------------------------------------------------------- */
/*
 * Initialises an existing node by setting its value, its parent, and
 * initialising its container for future children.
 */
static void
ptree_init_node(struct ptree_t* node, struct ptree_t* parent, void* value)
{
	memset(node, 0, sizeof *node);
	bsthv_init_bsthv(&node->children);
	node->parent = parent;
	node->value = value;
}

/*
 * Allocates a new root node with the key name "root". This function will
 * call ptree_init_ptree() to initialise the node.
 */
struct ptree_t*
ptree_create(void* value)
{
	struct ptree_t* tree;
	if(!(tree = (struct ptree_t*)MALLOC(sizeof(struct ptree_t))))
		return NULL;
	ptree_init_ptree(tree, value);
	return tree;
}

/* ------------------------------------------------------------------------- */
/*
 * Initialises an already allocated root node and gives it the name "root".
 * This function will call ptree_init_node() to initialise the node.
 */
void
ptree_init_ptree(struct ptree_t* tree, void* value)
{
	assert(tree);

	ptree_init_node(tree, NULL, value);
}

/* ------------------------------------------------------------------------- */
/*
 * Recursively destroys all nodes of a given root node, then de-allocates that
 * root node.
 */
void
ptree_destroy(struct ptree_t* tree)
{
	assert(tree);

	ptree_destroy_keep_root(tree);
	FREE(tree);
}

static const char*
ptree_get_node_key(const struct ptree_t* node)
{
	const char* key;
	if(node->parent && (key = bsthv_find_element(&node->parent->children, node)))
		return key;
	else
		return "root";
}

/* ------------------------------------------------------------------------- */
/*
 * Recursively destroys all children of a node, and also de-allocates the
 * tree's key and child container. This does NOT unlink the node from its
 * parent. Use ptree_destroy_keep_root() for that.
 */
static void
ptree_destroy_children_recurse(struct ptree_t* tree)
{
	/* destroy all children recursively */
	BSTHV_FOR_EACH(&tree->children, struct ptree_t, key, child)
		ptree_destroy_children_recurse(child);
		FREE(child);
	BSTHV_END_EACH
	bsthv_clear_free(&tree->children);

	/* free the data of this node, if specified */
	if(tree->value)
	{
		if(tree->free_value)
			tree->free_value(tree->value);
		else
		{
			fprintf(stderr, "ptree_destroy_recurse(): Unable to de-allocate value!"
				" No free() function was specified!");
			fprintf(stderr, " (at ptree node with name \"%s\")", ptree_get_node_key(tree));
			fprintf(stderr, "\nThe node will be de-allocated, the value will not.\n");
		}
	}
}

/*
 * Unlinks a node from its parent and recursively destroys all nodes, and also
 * de-allocates the root node's key and child container. You must use
 * ptree_init_ptree() on the node again before being able to re-use it as a new
 * ptree.
 */
void
ptree_destroy_keep_root(struct ptree_t* tree)
{
	assert(tree);

	/*
	 * If tree has parent, detach by removing ourselves from the parent's list
	 * of children.
	 */
	if(tree->parent)
		bsthv_erase_element(&tree->parent->children, tree);

	/* recursively destroy children of detached node */
	ptree_destroy_children_recurse(tree);
}

/* ------------------------------------------------------------------------- */
static struct ptree_t*
ptree_add_node(struct ptree_t* tree, const char* key, void* value)
{
	struct ptree_t* child;
	if(!(child = (struct ptree_t*)MALLOC(sizeof(struct ptree_t))))
		return NULL;

	if(!bsthv_insert(&tree->children, key, child))
	{
		FREE(child);
		return NULL;
	}

	ptree_init_node(child, tree, value);
	return child;
}

static struct ptree_t*
ptree_set_recurse(struct ptree_t* node, char* key, char** saveptr, void* value)
{
	/* Get next token and add it as a child of the current node. */
	char* child_key;
	struct ptree_t* child;
	if((child_key = strtok_r_portable(NULL, node_delim, saveptr))) /* we haven't reached the last
												* node yet, create any middle
												* nodes if they don't exist
												* yet, or get the current
												* middle node and recurse */
	{
		/* node doesn't exist, create */
		if(!(child = ptree_get_node_no_depth(node, key)))
		{
			if(!(child = ptree_add_node(node, key, NULL)))
				return NULL;
		}

		/* repeat process with child node */
		return ptree_set_recurse(child, child_key, saveptr, value);
	}

	/*
	 * If the token is NULL, then this is the last node to create. If it
	 * already exists we return NULL.
	 */
	if(!(child = ptree_add_node(node, key, value)))
		return NULL;
	return child;
}

/*
 * Adds a node to the given node, potentially filling in any missing middle
 * nodes. This function calls ptree_add_node_recurse()
 */
struct ptree_t*
ptree_set(struct ptree_t* root, const char* key, void* value)
{
	struct ptree_t* node;
	char* saveptr;
	char* key_tok;
	char* child_node_key;
	uint32_t child_count;

	assert(root);
	assert(key);

	/* prepare for tokenisation */
	if(!(key_tok = malloc_string(key)))
		return NULL;
	child_node_key = strtok_r_portable(key_tok, node_delim, &saveptr);

	/* store current child count so we can tell if malloc failed */
	child_count = bsthv_count(&root->children);

	/*
	 * Recursively fills in any middle nodes until the end node is reached and
	 * returns the leaf node
	 */
	node = ptree_set_recurse(root,
							 child_node_key,
							 &saveptr,
							 value);

	/*
	 * If the node wasn't added successfully and the children of root were
	 * modified, undo all changes.
	 * Note that ptree_get_node() uses malloc() because of strtok. Use
	 * ptree_get_node_no_depth() instead.
	 */
	if(!node && bsthv_count(&root->children) != child_count)
	{
		if((node = ptree_get_node_no_depth(root, child_node_key)))
			ptree_destroy(node);
		node = NULL;
	}

	/* child_node_key points into key_tok -- free key_tok after cleaning up */
	free_string(key_tok);

	return node;
}

/* ------------------------------------------------------------------------- */
/*
 * Sets the parent of a node and checks for cycles in the graph.
 */
char
ptree_set_parent(struct ptree_t* node, struct ptree_t* parent, const char* key)
{
	assert(node);
	assert(key);

	/* if parent is non-NULL, we need to do some inspections */
	if(parent)
	{
		/*
		 * Make sure that parent is independent of node, thus avoiding cycles.
		 * Note that it is perfectly valid for node to be a child of parent.
		 * Shifting nodes around in a tree is valid.
		 */
		if(node == parent || ptree_node_is_child_of(parent, node))
			return 0;

		/* insert into parent */
		if(!bsthv_insert(&parent->children, key, node))
			return 0;
	}

	/* remove from current parent */
	if(node->parent)
		bsthv_erase_element(&node->parent->children, node);

	/* set new parent */
	node->parent = parent;

	return 1;
}

/* ------------------------------------------------------------------------- */
char
ptree_remove(struct ptree_t* root, const char* key)
{
	struct ptree_t* node;

	assert(root);
	assert(key);

	if(!(node = ptree_get_node(root, key)))
		return 0;
	ptree_destroy(node);
	ptree_clean(root);
	return 1;
}

/* ------------------------------------------------------------------------- */
uint32_t
ptree_clean(struct ptree_t* root)
{
	uint32_t count = 0;

	assert(root);

	BSTHV_FOR_EACH(&root->children, struct ptree_t, key, child)
		count += ptree_clean(child);
		if(bsthv_count(&child->children) == 0 && child->value == NULL)
		{
			bsthv_clear_free(&child->children);
			FREE(child);
			BSTHV_ERASE_CURRENT_ITEM_IN_FOR_LOOP(&root->children, key, child);

			++count;
		}
	BSTHV_END_EACH

	return count;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_get_root(const struct ptree_t* node)
{
	assert(node);

	while(node->parent)
		node = node->parent;

	return (struct ptree_t*)node;
}

/* ------------------------------------------------------------------------- */
void
ptree_set_dup_func(struct ptree_t* node, ptree_dup_func func)
{
	assert(node);
	node->dup_value = func;
}

/* ------------------------------------------------------------------------- */
void
ptree_set_free_func(struct ptree_t* node, ptree_free_func func)
{
	assert(node);
	node->free_value = func;
}

/* ------------------------------------------------------------------------- */
static char
ptree_duplicate_children_into_existing_node_recurse(struct ptree_t* target,
													const struct ptree_t* source)
{
	/* duplicate source into target */
	target->dup_value = source->dup_value;
	target->free_value = source->free_value;
	if(source->value)
	{
		/* duplication function and free functions must exist */
		if(!source->dup_value || !source->free_value)
			return 0;

		/* duplicate value */
		if(!(target->value = source->dup_value(source->value)))
			return 0;
	}

	/* iterate over all children of source and duplicate them */
	BSTHV_FOR_EACH(&source->children, struct ptree_t, key, node)
		struct ptree_t* child;
		if(!(child = ptree_add_node(target, key, NULL)))
			return 0;  /* duplicate key error */
		if(!ptree_duplicate_children_into_existing_node_recurse(child, node))
			return 0;  /* some other error, propagate */
	BSTHV_END_EACH

	return 1;
}

char
ptree_duplicate_children_into_existing_node(struct ptree_t* target,
											const struct ptree_t* source)
{
	struct bsthv_t temp;

	assert(target);
	assert(source);

	/*
	 * In order to avoid circular copying, store all copied children in a
	 * temporary map before inserting them into the actual target tree.
	 */
	bsthv_init_bsthv(&temp);
	BSTHV_FOR_EACH(&source->children, struct ptree_t, key, node)

		/* try to duplicate node and insert into temp map */
		struct ptree_t* child = ptree_duplicate_tree(node);
		if(!child)
		{
			/* destroy temp nodes and clean up */
			BSTHV_FOR_EACH(&temp, struct ptree_t, h, dirty_node)
				ptree_destroy(dirty_node);
			BSTHV_END_EACH
			bsthv_clear_free(&temp);
			return 0;
		}
		if(!bsthv_insert(&temp, key, child))
		{
			/* destroy temp nodes and clean up */
			ptree_destroy(child);
			BSTHV_FOR_EACH(&temp, struct ptree_t, h, dirty_node)
				ptree_destroy(dirty_node);
			BSTHV_END_EACH
			bsthv_clear_free(&temp);
			return 0;
		}
	BSTHV_END_EACH

	/*
	 * Free to insert children of temp tree into target node. No need to check
	 * for cycles, they aren't possible.
	 */
	BSTHV_FOR_EACH(&temp, struct ptree_t, hash, node)
		node->parent = target;

		/*
		 * If we encounter a duplicate key, revert all insertions.
		 */
		if(!bsthv_insert(&target->children, hash, node))
		{
			BSTHV_FOR_EACH(&temp, struct ptree_t, h, dirty_node)
				if(node == dirty_node)
					break;
				/* if this assert fails, something seriously went wrong */
				assert(dirty_node == bsthv_erase(&target->children, h));
			BSTHV_END_EACH

			/* destroy temp nodes and clean up */
			BSTHV_FOR_EACH(&temp, struct ptree_t, h, dirty_node)
				ptree_destroy(dirty_node);
			BSTHV_END_EACH
			bsthv_clear_free(&temp);
			return 0;
		}
	BSTHV_END_EACH

	bsthv_clear_free(&temp);

	return 1;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_duplicate_tree(const struct ptree_t* source_node)
{
	struct ptree_t* new_root;

	assert(source_node);

	/*
	 * Create a new root, into which source_node is copied.
	 * Note that the value is being set to NULL, but will be overwritten
	 * in ptree_duplicate_children_into_existing_node().
	 */
	new_root = ptree_create(NULL);
	if(!new_root)
		return NULL;

	/* try duplicating */
	if(!ptree_duplicate_children_into_existing_node_recurse(new_root, source_node))
	{
		/* recursively free all nodes and values and return error */
		ptree_destroy(new_root);
		return NULL;
	}

	return new_root;
}

/* ------------------------------------------------------------------------- */
struct ptree_t*
ptree_get_node_no_depth(const struct ptree_t* tree, const char* key)
{
	assert(tree);
	assert(key);
	return bsthv_find(&tree->children, key);
}

/* ------------------------------------------------------------------------- */
static struct ptree_t*
ptree_get_node_recurse(const struct ptree_t* tree, char** saveptr)
{
	/*
	 * Get next token, hash, and search in current node. If the tree is NULL
	 * or if there are no more tokens, then we've found the node we're looking
	 * for.
	 */
	char* token;
	if((token = strtok_r_portable(NULL, node_delim, saveptr)) && tree)
	{
		struct ptree_t* child;
		child = bsthv_find(&tree->children, token);
		return ptree_get_node_recurse(child, saveptr);
	} else
		return (struct ptree_t*)tree;
}

struct ptree_t*
ptree_get_node(const struct ptree_t* tree, const char* key)
{
	struct ptree_t* result;
	char* saveptr;
	char* key_iter;

	assert(tree);
	assert(key);

	/* prepare key for tokenisation */
	key_iter = cat_strings(2, "n.", key); /* root key name is ignored, but must exist */
	if(!key_iter)
		return NULL;
	strtok_r_portable(key_iter, node_delim, &saveptr);

	result = ptree_get_node_recurse(tree, &saveptr);
	free_string(key_iter);
	return result;
}

/* ------------------------------------------------------------------------- */
char
ptree_node_is_child_of(const struct ptree_t* node,
					   const struct ptree_t* tree)
{
	assert(tree);

	BSTHV_FOR_EACH(&tree->children, struct ptree_t, key, n)
		if(n == node || ptree_node_is_child_of(node, n))
			return 1;
	BSTHV_END_EACH

	return 0;
}

/* ------------------------------------------------------------------------- */
static void
ptree_print_impl(const struct ptree_t* tree, uint32_t depth)
{
	char* value = "NULL";

	/* indentation */
	uint32_t i;
	for(i = 0; i != depth; ++i)
		printf("    ");

	/* print node info */
	if(tree->value)
		value = tree->value;
	printf("key: \"%s\", val: %s\n", ptree_get_node_key(tree), value);

	/* print children */
	BSTHV_FOR_EACH(&tree->children, struct ptree_t, key, child)
		ptree_print_impl(child, depth+1);
	BSTHV_END_EACH
}

void
ptree_print(const struct ptree_t* tree)
{
	ptree_print_impl(tree, 0);
}
