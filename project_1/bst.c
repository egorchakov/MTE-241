#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include "bst.h"

/*********************************************************************
 *
 * IMPLEMENTATION
 *
 *********************************************************************/

/*
  __inline bsn_t* bsn_create( S32 val ) 

  Creates a binary search tree  node (bsn) with a specified value.

  Parameters:
    S32 val
        A value to be assigned to the node.
*/
__inline bsn_t* bsn_create( S32 val ) {
    bsn_t* tmp = malloc(sizeof(bsn_t));             
    if (!tmp) _sys_exit(EXIT_FAILURE);                   
    tmp->val = val;                                 
    tmp->left = NULL;                               
    tmp->right = NULL;                              
    return tmp;
}

/*
 *  void bst_init( bst_t *tree )
 *
 *  Initializes a binary search tree.
 *
 *  Parameters:
 *      bst_t *tree
 *          Pointer to a tree that needs to be initialized.
*/
void bst_init( bst_t *tree ) {
    tree->root = NULL;
    tree->size = 0;
}

/*
 *  void bst_destroy( bst_t *tree ) 
 *
 *  Destroys a binary search tree.
 *
 *  Parameters:
 *      bst_t *tree
 *          Pointer to a tree that needs to be destroyed.
*/
void bst_destroy( bst_t *tree ) {
    // Calling recursive delete on root node
    bst_destroy_start_with_node(tree->root);
    free(tree);
}

/*
 *  void bst_destroy_start_with_node( bsn_t *node )
 *
 *  Destroys a (sub-)tree recursively.
 *
 *  Parameters:
 *      bsn_t *node
 *          A pointer to an arbitrary node in a tree. The subtree of which this 
 *          node is parent will be destroyed.
 */
void bst_destroy_start_with_node( bsn_t *node ){
    if (!node) return;
    if (node->left) bst_destroy_start_with_node(node->left);
    if (node->right) bst_destroy_start_with_node(node->right);
    free(node);
    node = NULL;
}

/*
 *  size_t bst_size( bst_t *tree )
 *  
 *  Returns the tree size if the tree exists and 0 otherwise.
 *  
 *  Parameters:
 *      bst_t *tree
 *          A pointer to a tree whose size needs to be known.   
*/
size_t bst_size( bst_t *tree ) {
    return (tree ? tree->size : 0);
}

/*
 *  bool bst_insert( bst_t *tree, S32 val )
 *  
 *  Inserts a value into a tree.
 *  
 *  Parameters:
 *      bst_t *tree
 *          A pointer to a tree into which the value needs to be insterted.
 *      
 *      S32 val
 *          Value that needs to be inserted.               
*/
bool bst_insert( bst_t *tree, S32 val ) {
    // Initialization
    bsn_t* new_node =  NULL;
    bsn_t* parent = NULL;
    bsn_t* visitor = NULL;
    new_node = bsn_create(val);

    // Non-existant tree
    if (!tree) return __FALSE;

    // Empty tree
    if (!tree->root) tree->root = new_node;

    // Tree exists and is not empty
    else {
        visitor = tree->root;

        // Keep descending and comparing values 
        // (current vs the one to be inserted)
        while(visitor){
            parent = visitor;
            visitor = (val > visitor->val) ? visitor->right : visitor->left;
        }
        
        // Decide which child should new_node be
        if (val > parent->val) parent->right = new_node;
        else parent->left = new_node;
    }

    ++tree->size;
    return __TRUE;
}

/*
 *  S32 bst_min( bst_t *tree )
 *  
 *  Returns the smallest value present in the tree if the tree is not empty and 
 *  INT_MAX otherwise.
 *  
 *  Parameters:
 *      bst_t *tree
 *          A pointer to a tree in which the value needs to be insterted.
*/
S32 bst_min( bst_t *tree ) {
    bsn_t* visitor = NULL;
    // Null or empty tree ==> maximum
    if (! (tree && tree->root)) return INT_MAX;
    visitor = tree->root;
    while (visitor->left) visitor = visitor->left;
    return visitor->val;
}
inserte
/*
 *  S32 bst_max( bst_t *tree )
 *  
 *  Returns the largest value present in the tree if the tree is not empty and 
 *  INT_MIN otherwise.
 *  
 *  Parameters:
 *      bst_t *tree
 *          A pointer to a tree in which the value needs to be insterted.
*/
S32 bst_max( bst_t *tree ) {
    bsn_t* visitor = NULL;
    // Null or empty tree ==> minimum
    if (! (tree && tree->root)) return INT_MIN;
    visitor = tree->root;
    while (visitor->right) visitor = visitor->right;
    return visitor->val;
}

/*
 *  bool bst_erase( bst_t *tree, S32 val )
 *  
 *  Deletes a value from a tree.
 *  
 *  Parameters:
 *      bst_t *tree
 *          A pointer to a tree from which the value needs to be deleted.
 *      
 *      S32 val
 *          Value that needs to be deleted.               
*/
bool bst_erase( bst_t *tree, S32 val ) {
    // Initialization
    bsn_t *parent = NULL;
    bsn_t *visitor = tree->root;
    bsn_t* maxLST = NULL;
    bsn_t* parentOfMaxLST = NULL;

    // Empty or non-existant tree
    if (!(tree && tree->root)) return __FALSE;

    // Determining the target node
    while( visitor->val != val ){
        parent = visitor;
        visitor = (val > visitor->val) ? visitor->right : visitor->left;
    }

    // 0 children case
    if (! (visitor->right || visitor->left) ){
        if (visitor == tree->root) tree->root = NULL;
        else if (parent->left == visitor) parent->left = NULL;
        else parent->right = NULL;
    }
    // 1 child case 
    // XOR expression from 
    // https://stackoverflow.com/questions/1596668/logical-xor-operator-in-c*/
    else if (!visitor->right != !visitor->left){
        if (visitor == tree->root){
            if (visitor->left) {
                tree->root = visitor->left;
                visitor->left = NULL;
            }

            else if (visitor->right){
                tree->root = visitor->right;
                visitor->right = NULL;
            }
        }

        else if (parent->left == visitor){
            parent->left = (visitor->left ? visitor->left : visitor->right);
        }

        else if (parent->right == visitor){
            parent->right = (visitor->left ? visitor->left : visitor->right);
        }
    }

    // 2 children case
    else {
        // Find max value in left subtree of visitor
        maxLST = visitor->left;
        while (maxLST->right){
            parentOfMaxLST = maxLST;
            maxLST = maxLST->right;
        }

        // Visitor's left child has a non-empty right subtree
        if (parentOfMaxLST){
            // At this point, maxLST can either have a left child or no children
            parentOfMaxLST->right = maxLST->left;
            maxLST->left = visitor->left;
            maxLST->right = visitor->right;
        }

        // Visitor's left child has an empty right subtree, 
        // i.e. it is the maxLST
        else maxLST->right = visitor->right;

        if (visitor == tree->root) tree->root = maxLST;
        else if (visitor == parent->left) parent->left = maxLST;
        else parent->right = maxLST;
    }

    free(visitor);
    --tree->size;
    return __TRUE;
}


/*********************************************************************
 *
 * TESTING
 *
 *********************************************************************/

#include <lpc17xx.h>
#include "GLCD.h"
#include "GLCD_Scroll.h"

static S32 value_array[100] = {
         534, 6415,  465, 4459, 6869, 4442, 5840, 4180, 7450, 9265,
          23, 2946, 3657, 3003,   29, 8922, 2199, 6973, 2344, 1802,
        9248, 5388, 2198, 2838, 1117, 5346, 4619, 8334, 9593, 2288,
        7346, 9252, 8169, 4138, 7479,  366, 5064, 6872,   -3, 8716,
        8089,   15, 5337, 8700, 8128, 6673, 5395, 7772, 5792, 3379,
        2438, 2184, 1176, 6083, 6572,  915, 1635, 6457, 3729, 7791,
        7642, 1548, 6267, 6562, 6477, 6026, 7460, 7226, 1994, 6519,
        7660, 3018, 2205,  559, 1347, 1647, 8778, 3864, 2543, 8370,
        1152,  865,  860, 8735, 4782,  574, 5858, 7089, 2096, 7449,
        1310, 3043, 8247, 6382, 2470, 3072, 1297, 7396, 7073,  140
    };

static S32 erase_array[5][20] = {
        { 915, 1802, 1994, 6083,  865, 8735, 6457, 8334, 4459, 3003,
         2198, 2470, 7642,   15, 7772, 1152,   29, 2096,  574, 6415},

        {7396, 5858, 4442, 6872, 8128, 2838,  465, 6477, 8247, 6572,
         2946, 1297, 3729, 4138, 5064, 8778, 4619, 5346,   -3, 3657},

        {1347, 2288, 7791, 7073, 5792, 3018,  366, 7449, 2543, 8089,
         4180, 6026, 3864, 5395, 7226, 1117,   23, 7089, 1635, 6267},

        {8700, 3072, 7660, 6673, 2438, 3043, 1548, 4782, 6519, 7460,
          559,  860, 6562, 9593, 1647, 1310, 3379, 8716, 8922, 7450},

        {9265, 6973, 8169, 5388,  140, 6869, 2344, 9252, 2184, 9248,
          534, 2199, 6382, 7479, 8370, 7346, 5337, 5840, 2205, 1176}
    };

int main( void ){
    // Initialization
    int i,j;
    bst_t* tree = NULL;
    
    SystemInit();
    init_scroll();
    GLCD_Clear( Blue );
    
    tree = malloc(sizeof(bst_t));
    bst_init(tree);
    
    // Insertion
    for (i=0; i<100; i++) bst_insert(tree, value_array[i]);
    printf("0 | %d,  %d\n",bst_min(tree), bst_max(tree));
            
    // Deletion            
    for (i=0; i<5; i++){
            for (j=0; j<20; j++) bst_erase(tree, erase_array[i][j]);
            printf("%d | %d, %d\n", i+1, bst_min(tree), bst_max(tree));
    }

    // Destruction
    bst_destroy(tree);
    while ( 1 ) {
        /* An emebedded system does not terminate... */
    }
}
