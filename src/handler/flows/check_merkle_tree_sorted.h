#pragma once

#include "os.h"
#include "cx.h"

#include "get_merkle_leaf_element.h"

#include "../../boilerplate/dispatcher.h"

// this flow aborts if any element is larger than this size
// In PSBT, keys are currently up to 1+78 (for a serialized extended public key).
#define MAX_CHECK_MERKLE_TREE_SORTED_PREIMAGE_SIZE 80

typedef struct {
    machine_context_t ctx;

    // input
    const uint8_t *root;
    size_t size;

    // internal state
    size_t cur_el_idx;
    int cur_el_len;
    uint8_t cur_el[MAX_CHECK_MERKLE_TREE_SORTED_PREIMAGE_SIZE];
    int prev_el_len;
    uint8_t prev_el[MAX_CHECK_MERKLE_TREE_SORTED_PREIMAGE_SIZE];

    union {
        get_merkle_leaf_element_state_t get_merkle_leaf_element;
    } subcontext;
} check_merkle_tree_sorted_state_t;


/**
 * Given a Merkle tree root and the size of the tree, it requests all the elements to the client (verifying Merkle
 * proofs) and verifies that the leaf preimages are in lexicographical order.
 * Fails with SW_INCORRECT_DATA if any at any key is not lexicographically strictly larger than the previous.
 */
void flow_check_merkle_tree_sorted(dispatcher_context_t *dispatcher_context);


/**
 * Convenience function to call the get_merkle_preimage flow.
 * TODO: more precise docs
 */
static inline void call_check_merkle_tree_sorted(dispatcher_context_t *dispatcher_context,
                                                 check_merkle_tree_sorted_state_t *flow_state,
                                                 command_processor_t ret_proc,
                                                 const uint8_t root[static 20],
                                                 size_t size)
{
    flow_state->root = root;
    flow_state->size = size;

    dispatcher_context->start_flow(
        flow_check_merkle_tree_sorted,
        (machine_context_t *)flow_state,
        ret_proc
    );
}
