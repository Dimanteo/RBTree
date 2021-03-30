/**
 * @file RBTree.h
 * @brief Red-black tree container and interfaces.
 * @author Kolobaev Dmitriy
 * 
 * 
 * This header contains user interfaces of RBTree class, 
 * which is red-black tree container.
 */
#ifndef RBTREE_H
#define RBTREE_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/// Type of values, stored in tree
typedef int value_t;

/// Red-black tree container class.
struct RBTree;

/**
 * @brief Constructor of class RBTree.
 * 
 * @return struct RBTree* Returns pointer to tree object, 
 * that should be used in subsequent calls. On error returns NULL.
 * @warning Allocates memory, so pointer should be freed via rbt_destruct().
 */
struct RBTree *rbt_init();

/**
 * @brief Destructor of class RBTree.
 * 
 * @param tree Pointer to object, that should be destroyed.
 * @return int 0 on success, -1 on error.
 */
int rbt_destruct(struct RBTree *tree);

/**
 * @brief Inserts value in tree.
 * 
 * @param tree Pointer to tree object.
 * @param val Value to insert.
 * @return int 1 if value inserted, 0 if value already was in tree, -1 on error.
 */
int rbt_insert(struct RBTree *tree, value_t val);

/**
 * @brief Checks if tree contains given value.
 * 
 * @param tree Pointer to tree object.
 * @param val Value to search for.
 * @return int 1 if contains, 0 if not contains or an error occured.
 */
int rbt_contains(const struct RBTree *tree, value_t val);

/**
 * @brief Removes value from tree
 * 
 * @param tree Pointer to tree object.
 * @param val Value to remove
 * @return int 1 if value removed, 0 if value wasn't found in tree, -1 on error.
 */
int rbt_remove(struct RBTree *tree, value_t val);

/**
 * @brief Tree iterator.
 * 
 * Applies callback to each value stored in tree in ascending order.
 * As parameters callback will receive value being processed,
 * pointer to tree object for which iterator was called and pointer void* data, 
 * that has been passed through parameters.
 * 
 * @param tree Pointer to tree object.
 * @param callback Pointer to callback function.
 * @param data Pointer to pass to callback function as parameter.
 * @return int 0 on success, -1 on error.
 * @warning Modifying tree in callback function leads to undefined behaviour.
 */
int rbt_foreach(struct RBTree *tree,
                void(*callback)(value_t, struct RBTree*, void*), void *data);

/**
 * @brief Get number of values stored in a tree.
 * 
 * @param tree Pointer to tree object.
 * @return size_t number of values stored in tree.
 */
size_t rbt_get_size(struct RBTree *tree);

/**
 * @brief Creates tree representation in dot format.
 * 
 * Available only in debug build. Creates tree representation on dot language 
 * and writes it to file. Used to obtain visual tree graph via graphviz dot.
 * 
 * @param tree Pointer to tree object.
 * @param filename Name of output file for dot language.
 */
void rbt_dump(struct RBTree *tree, const char* filename);

void malloc_fail_enable();
void malloc_fail_disable();

#endif /* RBTREE_H */
