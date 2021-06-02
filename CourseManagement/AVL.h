/* Class Implementing a Two-way Generic rank AVL BST, supporting search, addition and removal . */

#ifndef AVL_H
#define AVL_H



#include <iostream>
#include "Node.h"
#include "Utils.h"




template <class T>
class AVL
{


void unBalancedRemoval(Node<T>* target); //perform 'regular' search tree key removal on object, without maintaining balance
bool performRollCorrection(Node<T>* iterator); //perform roll correction on tree with root of the node iterator if necessary. return ture if roll has been performed, false otherwise

public:
Node<T>* root;
Node<T>* rightmost;
Node<T>* leftmost;

AVL(){root = nullptr; rightmost = nullptr; leftmost = nullptr;}
~AVL();
Status remove(const T& dest); // remove dest from object. 
Status add(const T& dest); // add single Node to tree
bool isEmpty(){return (!root);} //return '1' if tree is empty. '0' otherwise.
void correct(Node<T>* iterator);




};



template <class T>
AVL<T>::~AVL()
{
    delete root;
}

template <class T>
std::ostream& operator<<(std::ostream& os,const Node<T>* Node)
{
    if(!Node)
    {
        return os;
    }


    operator<<(os,(*Node).left);
    os<<(*Node).data<<',';
    operator<<(os,(*Node).right);
    return os;


}




template <class T>
Status AVL<T>::add(const T& dest)
{
    Node<T>* new_node = new Node<T>(dest); // bad_alloc exception would be handled outside of this scope

    if(this->isEmpty()) //handle first addition to tree 
    {
        (*this).root = new_node;
        (*this).rightmost = new_node;
        (*this).leftmost = new_node;
        return SUCCEEDED;
    }

    if((*root).find(dest))
    {
        return FAILED;
    }

    //update rightmost
    if(dest>(*rightmost).data)
    {
        rightmost = new_node;
    }

    //update leftmost
    if(dest<(*leftmost).data)
    {
        leftmost = new_node;
    }


    
    
    //Regular Binary Search Tree Insertion
    Node<T>* parent = (*root).find_insert_location(dest); //after calling find_insert_location, all height are updated 
    (dest < (*parent).data) ? (*parent).left = new_node : (*parent).right = new_node;
    (*new_node).parent = parent;

    //AVL tree Roll corrections
    Node<T>* iterator = parent;
    while (iterator)
    {
        if(performRollCorrection(iterator))
        {
            return SUCCEEDED;
        }
        iterator = iterator->parent;
    }

    return SUCCEEDED;
   
}

template <class T>
Status AVL<T>::remove(const T& dest)
{
    if(!root || !(*root).find(dest))
    {
        return FAILED;
    }


    Node<T>* target = (*this).root->find(dest); //find depletion target on tree
    if(dest == rightmost->data)//update rightmost to its father, if it's being removed
    {
        rightmost = target->parent ? target->parent : target->left;
    }
    if(dest == leftmost->data)//update leftmost to its father, if it's being removed
    {
        leftmost = target->parent ? target->parent : target->right;
    }
    Node<T>* iterator = target->parent;
    
    unBalancedRemoval(target);//regular binary tree key removal
    correct(iterator);
    
    return SUCCEEDED;
}

template <class T>
void AVL<T>::correct(Node<T>* iterator)
{
    if(!iterator)
    {
        return;
    }
    while(iterator)
    {
        int left_height = Node<T>::getHeight(iterator->left);
        int right_height = Node<T>::getHeight(iterator->right);
        iterator->height = 1 + max(right_height,left_height); //update height on path
        int pre_roll = iterator->height;
        if( performRollCorrection(iterator) && iterator->height == pre_roll)//if a roll has been performed and height remained unchanged, the tree is balanced
        {
            return;
        }
        iterator = iterator->parent;
    }


}

template <class T>
void AVL<T>::unBalancedRemoval(Node<T>* target)
{
    if(!(target->left || target->right)) //leaf case
    {
        if(target->parent) // if target is not root (single node in tree case)
        {
            target->parent->right == target ? target->parent->right = nullptr : target->parent->left = nullptr; // delete target as child
        }
        else
        {
            root = nullptr; //mark tree is empty
        }
        delete target;
    }
    
    else if (bool(target->left) != bool(target->right)) // single child case - connect son and granparent
    {
        Node<T>* child = !(target->left) ? target->right : target->left;
        if(target->parent)// if target is not root
        {
            target->parent->right == target ? target->parent->right = child : target->parent->left = child;
            child->parent = target->parent;
        }
        else
        {
            child->parent = nullptr;
            this->root = child;
        }
        
        
        target->left = nullptr; // to avoid recursive depletion of other nodes
        target->right = nullptr;
        delete target;
    }


    else //target has two children
    {
        Node<T>* sub = target->right->leftmost();
        rightmost = sub == rightmost ? rightmost->parent : rightmost;
        Node<T>* location  = sub->parent;
        Node<T>::swap(target,sub);
        unBalancedRemoval(sub); //only one recursive call- sub  must fit one of first two criterias
        correct(location);
          
    }
    
}

template <class T>
bool AVL<T>::performRollCorrection(Node<T>* iterator)
{
    int balance_factor =(*iterator).calc_BF();
    if(balance_factor == -2)
    {
        int bf_right = (*((*iterator).right)).calc_BF();
        if(bf_right ==1)
        {
            (*(*iterator).right).rightRoll();
            
        }
        if(iterator == root)
        {
            root = root->right;
        }
        iterator = (*iterator).leftRoll();
       
    
    }
    else if(balance_factor ==2)
    {
        int bf_left = (*((*iterator).left)).calc_BF();
        if(bf_left == -1)
        {
            (*(*iterator).left).leftRoll();
        }
        if(iterator == root)
        {
            root = root->left;
        }
        iterator = (*iterator).rightRoll();
        

        
    }
    else
    {
        return false;
    }

    return true;
    

}





#endif