#include <iostream>
#include<vector>


using namespace std;

// NOTE: In case of segmentation-fault just increase the order of the tree
//                          _   _
// keep in mind that MIN = |MAX/2|

#define MAX 15 // Maximum number of children a node can have
#define MIN 8 // Minimum number of children a node can have

// Blocks of the maze
struct block{
    int weight;
    vector<block*> neighbours;
    int shortestDistance;
    block *prevBlock;
};

/* All the functions before the "setNeighbours" function are used in the implementation of B-tree. */

struct btNode{
    block *keys[MAX + 1];         // Array of pointer to blocks stored in the B-tree node (aka keys of the node)
    int count;                     // Number of blocks in a B-tree node
    btNode *children[MAX + 1];  // Array of pointer to children blocks
};

// Creating the null root of the tree
btNode *root = nullptr;

btNode *createNewNode(block* key, btNode *child){
    // Creates a new B-tree node and returns the pointer to it
    btNode *newNode = new btNode;
    newNode->keys[1] = key;
    newNode->count = 1;
    newNode->children[0] = root;
    newNode->children[1] = child;
    return newNode;
}

void insertKeyIntoNode(block* key, int position, btNode *node, btNode *child) {
    // Adds the key to the node in a way that it remains sorted
    int i;
    for ( i = node->count; i > position; i-- ) {
        node->keys[i + 1] = node->keys[i];
        node->children[i + 1] = node->children[i];
    }

    node->keys[i + 1] = key;
    node->children[i + 1] = child;
    node->count++;
}

void splitNode(block *key, block **prevKey, int position, btNode *node, btNode *child, btNode **newNode) {
    // Splitting the node for insertion
    int splitPoint;
    int median = MIN;

    if (position > MIN)
        median = MIN + 1;

    splitPoint = median + 1;

    *newNode = new btNode;

    while (splitPoint <= MAX) {
        //  Moving all the right most nodes to a new node
        (*newNode)->keys[splitPoint - median]     = node->keys[splitPoint];
        (*newNode)->children[splitPoint - median] = node->children[splitPoint];
        splitPoint++;
    }

    node->count = median;
    (*newNode)->count = MAX - median;

    if (position <= MIN) {
        // Add key to left part
        insertKeyIntoNode(key, position, node, child);
    }

    else {
        // Add key to right part
        insertKeyIntoNode(key, position - median, *newNode, child);
    }

    *prevKey = node->keys[node->count];
    (*newNode)->children[0] = node->children[node->count];

    node->count--;
}

bool AddNewKey(block *newKey, block **prevValue, btNode *node, btNode **child) {
    // Adds the new key into the node

    int position;

    if(!node) {
        // There are no more nodes
        *prevValue = newKey;
        *child = NULL;
        return true;
    }

    if( newKey->shortestDistance < node->keys[1]->shortestDistance ) {
        // Key is smaller than the first key in the node
        position = 0;
    }

    else {
        // Go backwards until finding the new node's place in the node
        position = node->count;
        while(newKey->shortestDistance < node->keys[position]->shortestDistance && position > 1)
            position--;
    }

    if ( AddNewKey(newKey, prevValue, node->children[position], child) ) {
        // If the node has no children at that position
        // so the new key should be added to the node

        if (node->count < MAX) {
            // Node is not full -> add the key to this node
            insertKeyIntoNode(*prevValue, position, node, *child);
        }

        else {
            // Node is full -> split the node
            splitNode(*prevValue, prevValue, position, node, *child, child);
            return true;
        }
    }

    return false;
}

void insert(block *newBlock) {
    // Adds a new Block to the B-tree

    bool newRoot;
    block *i;
    btNode *child;

    newRoot = AddNewKey(newBlock, &i, root, &child);

    if(newRoot)
        // Creating new root
        root = createNewNode(i, child);
}

void removeKey(btNode *node, int position) {
    // Deletes the value from the node using its position
    // while keeping the node sorted

    for(int i= position + 1; i <= node->count; i++){
        node->keys[i - 1]     = node->keys[i];
        node->children[i - 1] = node->children[i];
    }
    node->count--;
}

void shiftKeyToChild(btNode *node, int position, char leftRight) {
    //Shifts block from parent to right child if 'r'
    // and to left child if 'l'

    // Does right shift
    if(leftRight == 'r'){
        btNode *x = node->children[position];
        int j = x->count;

        while (j > 0) {
            x->keys[j + 1]     = x->keys[j];
            x->children[j + 1] = x->children[j];
        }

        x->keys[1] = node->keys[position];
        x->children[1] = x->children[0];
        x->count++;

        x = node->children[position - 1];
        node->keys[position]     = x->keys[x->count];
        node->children[position] = x->children[x->count];
        x->count--;

        return;
    }

    // Does left shift
    if(leftRight == 'l'){
        int j = 1;
        btNode *x = node->children[position - 1];

        x->count++;
        x->keys[x->count]     = node->keys[position];
        x->children[x->count] = node->children[position]->children[0];

        x = node->children[position];
        node->keys[position] = x->keys[1];
        x->children[0]         = x->children[1];
        x->count--;

        for(j=1; j <= x->count; j++){
            x->keys[j]     = x->keys[j + 1];
            x->children[j] = x->children[j + 1];
        }

        return;
    }
}

void mergeNodes(btNode *node, int position) {
    // Merges two brothers that have less than MIN keys

    int j;

    btNode *rightNode = node->children[position];
    btNode *leftNode  = node->children[position - 1];

    leftNode->count++;
    leftNode->keys[leftNode->count]     = node->keys[position];
    leftNode->children[leftNode->count] = node->children[0];

    for(j=1; j <= rightNode->count; j++){
        leftNode->count++;
        leftNode->keys[leftNode->count]     = rightNode->keys[j];
        leftNode->children[leftNode->count] = rightNode->children[j];
    }

    for(j=position; j < node->count; j++){
        node->keys[j]     = node->keys[j + 1];
        node->children[j] = node->children[j + 1];
    }

    node->count--;
    free(rightNode);
}

void fixNode(btNode *node, int position) {
    // Fixes the given node

    if (!position) {
        if (node->children[1]->count > MIN) {

            shiftKeyToChild(node, position, 'l');
        }
        else {
            mergeNodes(node, 1);
        }
    }

    else {
        if (node->count != position) {
            if (node->children[position - 1]->count > MIN) {
                shiftKeyToChild(node, position, 'r');
            }

            else {
                if (node->children[position + 1]->count > MIN) {
                    shiftKeyToChild(node, position, 'l');
                }

                else {
                    mergeNodes(node, position);
                }
            }
        }

        else {
            if (node->children[position - 1]->count > MIN)
                shiftKeyToChild(node, position, 'r');
            else
                mergeNodes(node, position);
        }
    }
}

void replaceSuccessor(btNode *deadNode, int position) {
    // Replaces the successor for the node to be deleted (aka deadNode)

    btNode *dummy = deadNode->children[position];

    while(dummy->children[0] != nullptr)
        dummy = dummy->children[0];

    deadNode->keys[position] = dummy->keys[1];

}

bool removeKey(block *deadBlock,btNode *node) {
    // Removes the deadBlock from the node

    bool flag = false;
    int position;

    if (node) {   // If root is not empty

        if (deadBlock->shortestDistance < node->keys[1]->shortestDistance) {
            // Key is smaller than the first key in the node
            position = 0;
            flag = false;
        }

        else {
            // Go backwards until finding right position
            position = node->count;
            while((deadBlock->shortestDistance  <= node->keys[position]->shortestDistance) &&
                (deadBlock  != node->keys[position])  &&
                (position > 1))

                 position--;

            if ((deadBlock  == node->keys[position]) &&
                (deadBlock->shortestDistance  == node->keys[position]->shortestDistance)) {
            // To avoid deleting the wrong duplicate
                flag = true;
            }
            else {
                flag = false;
            }
        }

        if (flag) {
            // The brother node does not have enough keys to lend
            if (node->children[position - 1]) {
                replaceSuccessor(node, position);
                flag = removeKey(node->keys[position], node->children[position]);
            }
            else {
                removeKey(node, position);
            }
        }

        else {
            flag = removeKey(deadBlock, node->children[position]);
        }

        if (node->children[position] &&
           (node->children[position]->count < MIN)) {
            // The children become less than MIN so the node should be fixed
            fixNode(node, position);
        }
    }

    return flag;
}

void remove(block *deadBlock, btNode *node) {
    // Takes a pointer to a block in the B-tree and deletes it

    btNode *temp;

    if(!removeKey(deadBlock, node))
        return;

    // node becomes empty so we free up the memory
    if( node->count == 0 ){
            temp = node;
            node = node->children[0];
            free(temp);
    }

    root = node;
}
block *popMin(btNode *root) {
    // Deletes the minimum key and returns it
    block *min;
    btNode *ptr = root;

    while(ptr->children[0] != NULL){
        ptr = ptr->children[0];
    }

    min = ptr->keys[1];

    remove(min, root);
    return min;
}

bool isEmpty(btNode *root) {
    // Returns true if the B-tree is empty
    if(root == nullptr)
        return true;
    else
        return false;
}

void setNeighbours(vector<vector<block*>> &maze, int row, int col){
    // Creates a vector of pointers to the neighbours of each block and
    // sets it to maze[i]->neighbours for node i

    int weight;
    vector<block*> neighbours;

    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            weight = maze[i][j]->weight;

            if(weight > 0){ // if not a dead-end

                if((j-1>0) && (maze[i][j-1]->weight > 0)){
                    neighbours.push_back(maze[i][j-1]);     // left neighbour
                    }

                if((j+1<col) && (maze[i][j+1]->weight > 0)){
                    neighbours.push_back(maze[i][j+1]);     // right neighbour
                    }

                if((i+1<row) && (maze[i+1][j]->weight > 0)){
                    neighbours.push_back(maze[i+1][j]);     // down neighbour
                    }

                maze[i][j]->neighbours = neighbours;
                neighbours.clear();

                }
        }
    }
}

void bTreeDijkstra(vector<vector<block*>> &maze, block *source, int row, int col){
    /* Takes a 2D vector of blocks and sets the block->shortestDistance and
       block->prevBlock using Dijkstra algorithm that is implemented using a B-tree
       instead of a priority queue. */

    block *currentBlock = nullptr;
    int newDist = 0;

    source->shortestDistance = 0;

    insert(source); // Insert into B-tree

    while( !isEmpty(root) ){
        currentBlock = popMin(root);

        for( auto neighbour : currentBlock->neighbours ){
            newDist = currentBlock->shortestDistance + neighbour->weight;

            if( newDist < neighbour->shortestDistance ){
                // a shorter distance is found
                remove(neighbour, root);
                neighbour->shortestDistance = newDist;
                neighbour->prevBlock = currentBlock;
                insert(neighbour);
                // Note: We are removing and inserting again instead of changing
               //        the key so we don't mess up the search tree properties.
                }
            }
        }
    }

void solveMaze(vector<vector<block*>> &maze, int row, int col, block *source){
    // After setting the shortest distance and previous block for each node
    // this helper function changes the nodes on the shortest path to -1

    bTreeDijkstra(maze, source, row, col);

    int i = row-1;
    int j = col-1 ;
    block *goal;

    while( maze[i][j]->weight==0 ){
        j--;
    }

    goal = maze[i][j];

    block *ptr = goal;

    while( ptr->prevBlock != nullptr ){
        ptr->weight = -1;
        ptr = ptr->prevBlock;
    }

    source->weight = -1;
}

void printSolvedMaze(vector<vector<block*>> &maze, int row, int col){
    // Prints the maze after it is solved
    for( int i=0; i<row; i++ ){
        for( int j=0; j<col; j++ ){
            cout<<maze[i][j]->weight<<" ";
        }
    cout<<endl;
    }
}


int main(){

    int col, row, weight;
    block *source;
    cin>> row;
    cin>> col;


    vector<vector<block*>> maze;
    vector<block*> tempRow;

    // Inputs the plain maze into a 2D vector of pointers to blocks
    for(int i=0; i<row; i++){
        for(int j=0; j<col; j++){
            cin>>weight;
            block *newBlock = new block;

            newBlock->weight = weight;
            newBlock->prevBlock = nullptr;
            newBlock->shortestDistance = 100000000;
            newBlock->neighbours = {};
            tempRow.push_back(newBlock);
        }
    maze.push_back(tempRow);
    tempRow.clear();
    }


    // Finding the source (aka starting) block
    for(int i=0; i<col; i++){
        if(maze[0][i]->weight==1){
            source = maze[0][i];
            break;
        }
    }

    // Find the neighbours for each node
    setNeighbours(maze, row, col);

    // Solve the maze using B-tree Dijkstra
    solveMaze(maze, row, col, source);

cout<<"-------------------------"<<endl;
cout<<"------|Solved Maze|------"<<endl;

cout<<"-------------------------"<<endl;

    // Print the solved maze
   printSolvedMaze(maze, row, col);

    return 0;
}



/*


Some test cases to try
----------------------
4 4
0 1 0 0
0 1 1 1
1 2 3 2
0 0 1 0

8 10
0 0 0 1 0 0 0 0 0 0
0 3 1 1 0 1 51 0 1 0
0 2 0 1 0 0 1 41 1 0
0 10 0 1 1 111 1 0 0 0
0 34 1 1 0 0 1 23 1 0
0 1 0 5 0 1 1 0 0 0
0 1 0 166 1 0 54 1 1 0
0 0 0 0 0 0 0 7 0 0

7 11
0 0 0 0 0 0 0 1 0 0 0
0 0 3 2 5 3 2 5 6 4 2
0 1 1 7 6 1 1 7 9 0 3
1 1 4 0 1 9 4 8 6 0 2
1 1 4 1 9 9 1 8 3 4 2
0 3 1 1 2 1 1 2 1 0 0
0 0 0 0 1 0 0 0 0 0 0

*/
