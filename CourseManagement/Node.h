#ifndef NODE_H
#define NODE_H



template <class T>
struct Node
{
T data;
Node<T>* left;
Node<T>* right;
Node<T>* parent;
int height;



Node(const T& data = T()) : data(data){left = nullptr; right=nullptr; parent = nullptr; height =0;}
~Node();
Node<T>* find(const T& search_data); 
Node<T>* find_insert_location(const T& search_data); // Find parent Node under which a new Node with 'search_data' data field would be inserted
static int getHeight(const Node<T>* Node);                                                     // Function will also update all heights in insertion path
int calc_BF(){return getHeight(left)-getHeight(right);} // return object's balance factor
Node<T>* rightRoll(); 
Node<T>* leftRoll();
Node<T>* leftmost();
static void swap (Node<T>* node1, Node<T>* node2);


};

template <class T>
inline T max(T a, T b) // return larger of two template elements
{
    return a>b? a:b;
}

template <class T>
Node<T>::~Node()
{
    //parent = nullptr;
    delete left;
    delete right;


}

template <class T>
int Node<T>::getHeight(const Node<T>* Node)
{
    if(!Node)
    {
        return -1;
    }

    return (*Node).height;
}

template <class T>
Node<T>* Node<T>::find(const T& search_data) 
{
    if(search_data == data)
    {
        return this;
    }
    if (search_data > data)
    {
        return (!right)?nullptr:(*right).find(search_data);
    }
    return (!left)?nullptr:(*left).find(search_data);
}

template<class T>
Node<T>* Node<T>::find_insert_location(const T& search_data)
{
    //assuming search data is not currently in the tree, and tree is not empty
    Node<T>* next_step = search_data<data ? left : right;
    if(!(next_step))
    {
        (*this).height = 1; // parent of inserted node will always have height of 1 after insertion to a previously valid AVL tree
        return this;
    }
    
    Node<T>* insert_location = (*next_step).find_insert_location(search_data); 
    int left_height = getHeight(left);
    int right_height = getHeight(right);
    (*this).height = (1+ max(left_height,right_height)); // update current node's height
    return insert_location;
}


template<class T>
Node<T>* Node<T>::rightRoll()
{
    Node<T>* left_orig = left;
    if(parent)
    {
        
        this == (*parent).left ?  (*parent).left = left_orig :  (*parent).right = left_orig;// make left_orig relevant child of its granparent
    }
    
    (*left_orig).parent = parent;
    left = (*left_orig).right;
    (*left_orig).right = this;
    parent = left_orig;
    if(left)
    {
        (*left).parent = this;
    }

    //update heights
    this->height =  1 + max(getHeight(this->left),getHeight(this->right));
    left_orig->height = 1 + max(getHeight(left_orig->left),getHeight(left_orig->right));

    return left_orig;
}


template<class T>
Node<T>* Node<T>::leftRoll()
{
    Node<T>* right_orig = right;
    if(parent)
    {
        this == (*parent).left ?  (*parent).left = right_orig :  (*parent).right = right_orig;// make right_orig relevant child of its granparent
    }
    
    (*right_orig).parent = parent;
    right = (*right_orig).left;
    (*right_orig).left = this;
    parent = right_orig;
    if(right)
    {
         (*right).parent = this;

    }

    //update heights
    this->height =  1 + max(getHeight(this->left),getHeight(this->right));
    right_orig->height = 1 + max(getHeight(right_orig->left),getHeight(right_orig->right));
   
    return right_orig;
   
}

template <class T>
Node<T>* Node<T>::leftmost() // return the leftmost leaf in object
{
    if(!this->left)
    {
        return this;
    }

    return left->leftmost();

}

template <class T>
void Node<T>::swap (Node<T>* node1, Node<T>* node2)
{
    T temp = node1->data;
    node1->data = node2->data;
    node2->data = temp; 

}


#endif