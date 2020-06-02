#ifndef quadTree_h
#define quadTree_h

#include <GLUT/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Tree structure
struct treeNode_ {
    double centerX;
    double centerY;
    double nodeMass;
    int numOfPar;
    int isLeaf;
    double Lborder;
    double Rborder;
    double Uborder;
    double Dborder;
    struct treeNode_* NW;
    struct treeNode_* NE;
    struct treeNode_* SW;
    struct treeNode_* SE;
    int id;
};
typedef struct treeNode_ treeNode;



// Declarations
void create_tree(const int N);
void insert_tree(int i, treeNode* node);
void free_tree(treeNode* node);
void print_tree(treeNode* node, int level);
void get_CenterOfMass(treeNode* node);
void make_children(treeNode *node);
double *x,*y,*mass,*forceX,*forceY,*u,*v;
//int N;
treeNode* rootNode;



#endif
