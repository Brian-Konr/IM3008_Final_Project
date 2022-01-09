// Searching on a B+ tree in C++

#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
using namespace std;
int MAX = 3;

// BP node
class Node {
  bool IS_LEAF;
  int *key, size, height;
  Node **ptr;
  friend class BPTree;

   public:
  Node();
};

// BP tree
class BPTree {
  Node *root;
  void insertInternal(int, Node *, Node *);
  Node *findParent(Node *, Node *);

   public:
  BPTree();
  void search(int);
  void insert(int);
  void display(Node *);
  Node *getRoot();
};

Node::Node() {
  key = new int[MAX];
  ptr = new Node *[MAX + 1];
}

BPTree::BPTree() {
  root = NULL;
}

// Search operation
void BPTree::search(int x) {
  if (root == NULL) {
    cout << "Tree is empty\n";
  } else {
    Node *cursor = root;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    for (int i = 0; i < cursor->size; i++) {
      if (cursor->key[i] == x) {
        cout << "Found\n";
        return;
      }
    }
    cout << "Not found\n";
  }
}

// Insert Operation
void BPTree::insert(int x) {
  if (root == NULL) {
    root = new Node;
    root->key[0] = x;
    root->IS_LEAF = true;
    root->size = 1;
  } else {
    Node *cursor = root;
    Node *parent;
    while (cursor->IS_LEAF == false) {
      parent = cursor;
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    if (cursor->size < MAX) {
      int i = 0;
      while (x > cursor->key[i] && i < cursor->size)
        i++;
      for (int j = cursor->size; j > i; j--) {
        cursor->key[j] = cursor->key[j - 1];
      }
      cursor->key[i] = x;
      cursor->size++;
      cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
      cursor->ptr[cursor->size - 1] = NULL;
    } else {
      Node *newLeaf = new Node;
      // 先把x插入
      // int virtualNode[MAX + 1];
      vector<int> virtualNode(MAX+1);
      for (int i = 0; i < MAX; i++) {
        virtualNode[i] = cursor->key[i];
      }
      int i = 0, j;
      while (x > virtualNode[i] && i < MAX)
        i++;
      for (int j = MAX + 1; j > i; j--) {
        virtualNode[j] = virtualNode[j - 1];
      }
      virtualNode[i] = x;
      // // debug
      // for (int i = 0; i < MAX+1; i++) {
      //   cout << virtualNode[i] << " ";
      // }
      // cout << "\n";

      newLeaf->IS_LEAF = true;
      cursor->size = (MAX + 1) / 2;
      newLeaf->size = MAX + 1 - (MAX + 1) / 2;
      cursor->ptr[cursor->size] = newLeaf;
      newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
      cursor->ptr[MAX] = NULL;
      for (i = 0; i < cursor->size; i++) {
        cursor->key[i] = virtualNode[i];
      }
      for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
        newLeaf->key[i] = virtualNode[j];
      }

      // // debug
      // cout << "cursor ------------\n";
      // for (int i = 0; i < cursor->size; i++) {
      //   cout << cursor->key[i] << " ";
      // }
      // cout << "\n";

      // // debug
      // cout << "newLeaf ------------\n";
      // for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
      //   cout << newLeaf->key[i] << " ";
      // }
      // cout << "\n";

      if (cursor == root) {
        Node *newRoot = new Node;
        newRoot->key[0] = newLeaf->key[0];
        newRoot->ptr[0] = cursor;
        newRoot->ptr[1] = newLeaf;
        newRoot->IS_LEAF = false;
        newRoot->size = 1;
        root = newRoot;
      } else {
        insertInternal(newLeaf->key[0], parent, newLeaf);
      }
    }
  }
}

// Insert Operation
void BPTree::insertInternal(int x, Node *cursor, Node *child) {
  if (cursor->size < MAX) {
    int i = 0;
    while (x >= cursor->key[i] && i < cursor->size)
      i++;
    for (int j = cursor->size; j > i; j--) {
      cursor->key[j] = cursor->key[j - 1];
    }
    for (int j = cursor->size + 1; j > i + 1; j--) {
      cursor->ptr[j] = cursor->ptr[j - 1];
    }
    cursor->key[i] = x;
    cursor->size++;
    cursor->ptr[i + 1] = child;
  } else {
    Node *newInternal = new Node;
    vector<int> virtualKey(MAX+1);
    vector<Node *> virtualPtr(MAX+2);
    for (int i = 0; i < MAX; i++) {
      virtualKey[i] = cursor->key[i];
    }
    for (int i = 0; i < MAX + 1; i++) {
      virtualPtr[i] = cursor->ptr[i];
    }
    int i = 0, j;
    while (x >= virtualKey[i] && i < MAX) // not sure
      i++;
    for (int j = MAX + 1; j > i; j--) {
      virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = x;
    // // // debug
    // cout << "virtualKey ------------\n";
    // for (int i = 0; i < MAX+1; i++) {
    //   cout << virtualKey[i] << " ";
    // }
    // cout << "\n";

    for (int j = MAX + 2; j > i + 1; j--) {
      virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child;
    // 以上插入virtual node完畢

    // 以下做splitting
    newInternal->IS_LEAF = false;
    cursor->size = (MAX + 1) / 2;
    newInternal->size = MAX - (MAX + 1) / 2;
    for (i = 0; i < cursor->size; i++) {
      // cout << virtualKey[i] << "\n";
      cursor->key[i] = virtualKey[i];
    }
    // cout << cursor->key[0] << "\n";
    for (i = 0; i < cursor->size + 1; i++) {
      cursor->ptr[i] = virtualPtr[i];
    }
    for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
      newInternal->key[i] = virtualKey[j];
    }
    for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
      newInternal->ptr[i] = virtualPtr[j];
    }
    if (cursor == root) {
      Node *newRoot = new Node;
      newRoot->key[0] = cursor->key[cursor->size];
      newRoot->ptr[0] = cursor;
      newRoot->ptr[1] = newInternal;
      newRoot->IS_LEAF = false;
      newRoot->size = 1;
      root = newRoot;
    } else {
      insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
    }
  }
}

// Find the parent
Node *BPTree::findParent(Node *cursor, Node *child) {
  Node *parent;
  if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
    return NULL;
  }
  for (int i = 0; i < cursor->size + 1; i++) {
    if (cursor->ptr[i] == child) {
      parent = cursor;
      return parent;
    } else {
      parent = findParent(cursor->ptr[i], child);
      if (parent != NULL)
        return parent;
    }
  }
  return parent;
}

// Print the tree
void BPTree::display(Node *cursor) {
  queue<Node*> q;
  cursor -> height = 0;
  q.push(cursor);
  int current_h = 0;
 
  // cout << "current -> key [i]: " << cursor -> key [0] << "\n";
  while(!q.empty()){
    Node* current = q.front();
    if(current -> height > current_h){
      current_h++;
      cout << "\n";
    }
    cout << "|" ;
    for (int i = 0; i < current->size; i++) {
      cout << " " << current -> key [i] << " ";
    }
    cout << "|" ;
    
    q.pop();
    if (current->IS_LEAF != true) {
      for (int i = 0; i < current->size+1; i++) {
        current -> ptr[i] -> height = current -> height+1;
        q.push(current -> ptr[i]);
      }
    }
  }
}


// Get the root
Node *BPTree::getRoot() {
  return root;
}

void bubble_sort(int array[], int n) {
    for (int i=0; i<n-1; i++) {
        for (int j=0; j<n-i-1; j++) {
            if (array[j] > array[j+1]) {
                int temp = array[j];
                array[j] = array[j+1];
                array[j+1] = temp;
            }
        }
    }
}

int main() {

  cout << "please type the max degree: ";
  cin >> MAX;
  while(MAX <= 2) {
    cout << "max degree should bigger than 2\n";
    cout << "please type the max degree: ";
    cin >> MAX;
  }
  MAX --;

  cout << "Please choose the insertion method\n";
  cout << "1: Insert the value one by one\n";
  cout << "2: Build the B+ Tree bottom-up\n";

  int method = 2; // default is using method 2
  cin >> method;
  while(method != 1 && method != 2) {
    cout << "Please enter 1 or 2 only!!\n";
    cin >> method;
  }

  // read file
  string filename("input.txt");
  int number;
  vector<int> inputValue;

  ifstream input_file(filename);

  if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    while (input_file >> number) {
        inputValue.push_back(number);
  }

  //end reading
	BPTree node;
	

  int* array = &inputValue[0];
	int arr_size = inputValue.size();

  if(method == 2) bubble_sort(array, arr_size);

  // // debug
  // for(int i = 0 ; i < arr_size; i ++ ){
	// 	cout << array[i] << " ";
	// }
  // cout << "\n";
	
	for(int i = 0 ; i < arr_size; i ++ ){
    // // debug
    // cout << array[i] << "\n";
		node.insert(array[i]);
    // // debug
    // cout << "\n--------------------\n";
    // node.display(node.getRoot());
    // cout << "\n--------------------\n";
	}

	node.display(node.getRoot());
}