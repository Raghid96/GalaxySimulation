#include "quadTree.h"
/*void main(){

	}
*/
void create_tree(const int N)
{
    rootNode -> numOfPar = 0;
    rootNode -> Dborder = 0.0;
    rootNode -> Uborder = 1.0;
    rootNode -> Lborder = 0.0;
    rootNode -> Rborder = 1.0;
    rootNode -> NW = NULL;
    rootNode -> NE = NULL;
    rootNode -> SW = NULL;
    rootNode -> SE = NULL;
    rootNode -> id = -1;
    rootNode -> isLeaf = 0;
    
    for (int i = 0; i < N; i++) {
        insert_tree(i, rootNode);
    }
}




void insert_tree(int i, treeNode* node)
{
    // CKECING THE IF THE NODE IS ROOTNODE    
    
    if (node == rootNode) rootNode -> numOfPar++;
    
    
    // create children if they do not exist
    if (!node -> SW) {
        
        // Allocate the memory FOR THE CHILDREN
        node -> NW = (treeNode*)malloc(sizeof(treeNode));
        node -> SW = (treeNode*)malloc(sizeof(treeNode));
        node -> NE = (treeNode*)malloc(sizeof(treeNode));
        node -> SE = (treeNode*)malloc(sizeof(treeNode));
        
        node -> id = -1; // set id of nodes without children to -1
        node -> isLeaf = 0;
        double midX = (node->Lborder + node->Rborder)/2;
        double midY = (node->Dborder + node->Uborder)/2;
        
        // Creating the children
        // NW
        node -> NW -> Dborder = midY;
        node -> NW -> Uborder = node->Uborder;
        node -> NW -> Lborder = node->Lborder;
        node -> NW -> Rborder = midX;
        make_children(node->NW);
        // SW
        node -> SW -> Dborder = node->Dborder;
        node -> SW -> Uborder = midY;
        node -> SW -> Lborder = node->Lborder;
        node -> SW -> Rborder = midX;
        make_children(node->SW);
        // NE
        make_children(node->NE);
        node -> NE -> Dborder = midY;
        node -> NE -> Uborder = node->Uborder;
        node -> NE -> Lborder = midX;
        node -> NE -> Rborder = node->Rborder;
        // SE
        node -> SE -> Dborder = node->Dborder;
        node -> SE -> Uborder = midY;
        node -> SE -> Lborder = midX;
        node -> SE -> Rborder = node->Rborder;
        make_children(node->SE);
    }
    // Particle on the WEST
    if (x[i] < (node->Lborder + node->Rborder)/2) {
        // Particle on the SOUTH
        if (y[i] < (node->Dborder + node->Uborder)/2) {
            if (node -> SW -> numOfPar == 0) {
                node -> SW -> id = i;
                node -> SW -> numOfPar++;
            } else if (node -> SW -> numOfPar == 1) {
                node -> SW -> numOfPar++;
                insert_tree(node->SW->id, node->SW);
                insert_tree(i, node->SW);
            } else {
                insert_tree(i, node->SW);
            }
        // Particle on the NORTH
        } else {
            if (node -> NW -> numOfPar == 0) {
                node -> NW -> id = i;
                node -> NW -> numOfPar++;
            } else if (node -> NW -> numOfPar == 1) {
                node -> NW -> numOfPar++;
                insert_tree(node->NW->id, node->NW);
                insert_tree(i, node->NW);
            } else {
                insert_tree(i, node->NW);
            }
        }
    }
    // Particle on the EAST
    else {
        // Particle on the SOUTH
        if (y[i] < (node->Dborder + node->Uborder)/2) {
            if (node -> SE -> numOfPar == 0) {
                node -> SE -> id = i;
                node -> SE -> numOfPar++;
            } else if (node -> SE -> numOfPar == 1) {
                node -> SE -> numOfPar++;
                insert_tree(node->SE->id, node->SE);
                insert_tree(i, node->SE);
            } else {
                insert_tree(i, node->SE);
            }
        }
        // Particle on the NORTH
        else {
            if (node -> NE -> numOfPar == 0) {
                node -> NE -> id = i;
                node -> NE -> numOfPar++;
            } else if (node -> NE -> numOfPar == 1) {
                node -> NE -> numOfPar++;
                insert_tree(node->NE->id, node->NE);
                insert_tree(i, node->NE);
            } else {
                insert_tree(i, node->NE);
            }
        }
    }
}

// Make the children
void make_children(treeNode *node){
    node -> numOfPar = 0;
    node  -> NW = NULL;
    node  -> NE = NULL;
    node  -> SW = NULL;
    node  -> SE = NULL;
    node  -> id = -1;
    node -> isLeaf = 1;
}

// Free the tree
void free_tree(treeNode* node) {
    if(!node->isLeaf) {free_tree(node->NW);
        free_tree(node->NE);
        free_tree(node->SW);
        free_tree(node->SE);
    }
    free(node);
}

// Printing the tree
void print_tree(treeNode* node, int level) {
    for(int i = 0; i < level; i++) printf("   ");
    printf("x=%f y=%f m=%f id=%d #=%d\n", node->centerX, node->centerY, node->nodeMass, node->id, node->numOfPar);
    
    if(node->NW){
        print_tree(node->NW, level+1);
    }
    if(node->NE){
        print_tree(node->NE, level+1);
    }
    if(node->SW){
        print_tree(node->SW, level+1);
    }
    if(node->SE){
        print_tree(node->SE, level+1);
    }
}

void get_CenterOfMass(treeNode* node)
{
    if (!node->NE) {
        if (node -> numOfPar == 0) {
            node -> nodeMass = 0;
            node -> centerX = 0;
            node -> centerY = 0;
        } else {
            node -> nodeMass = mass[node->id];
            node -> centerX = x[node->id];
            node -> centerY = y[node->id];
        }
    } else {
        get_CenterOfMass(node -> NW);
        get_CenterOfMass(node -> NE);
        get_CenterOfMass(node -> SW);
        get_CenterOfMass(node -> SE);
        
        // The totalmass
        node -> nodeMass =  node->NW->nodeMass +
                            node->NE->nodeMass +
                            node->SW->nodeMass +
                            node->SE->nodeMass;
        
        // The mass multiplied by the distance to origin (X - coordinate)
        node -> centerX =   node->NW->nodeMass * node->NW->centerX +
                            node->NE->nodeMass * node->NE->centerX +
                            node->SW->nodeMass * node->SW->centerX +
                            node->SE->nodeMass * node->SE->centerX;
        // X coordinate of center of mass
        node -> centerX = node->centerX / node->nodeMass;
        
         // The mass multiplied by the distance to origin (y coordinate)
        node -> centerY =   node->NW->nodeMass * node->NW->centerY +
                            node->NE->nodeMass * node->NE->centerY +
                            node->SW->nodeMass * node->SW->centerY +
                            node->SE->nodeMass * node->SE->centerY;
        // Y coordinate of center of mass
        node -> centerY = node->centerY / node->nodeMass;
    }
}



























