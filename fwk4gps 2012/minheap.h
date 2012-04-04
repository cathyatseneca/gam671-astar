/**********************************************************/
/*                                                        */
/* Author: Catherine Leung                                */
/*                                                        */
/* This data structure is a min heap. After inserting     */
/* values into this data structure, the smallest value    */
/* is always removed first                                */
/* Insertions are written to O(log N)                     */
/* Removes are written to O(logN)                         */
/* where N is number of items in the heap.                */
/*                                                        */
/* template MinHeap class.  The unknown data type, must   */
/* support <, > , <=, >=                                  */
/**********************************************************/
template <class T>
class MinHeap{
  T* data_;
  int cap_;
  int sz_;
public:
  MinHeap();
  void insert(const T&);  //inserts a value into the heap
  T remove();             //removes and returns smallest item from the heap
  bool isempty(){return sz_==0;}
  ~MinHeap(){delete [] data_;}
  void Print() const{
    for(int i=0;i<sz_;i++){
      cout << data_[i] << endl;
    }
  }
};
template <class T>
MinHeap<T>::MinHeap(){
  data_=new T[100];
  cap_=100;
  sz_=0;
}
template <class T>
void MinHeap<T>::insert(const T& curr){
  int empty=sz_;
  bool found=false;
  int parent=0;

  if(sz_ >= cap_){
    T* tmp=new T[cap_+100];
    for(int i=0;i<sz_;i++){
      tmp[i]=data_[i];
    }
    delete [] data_;
    data_=tmp;
  }
  while(empty!=0 && !found){
    parent=(empty-1)/2;
    if(data_[parent] > curr){
      data_[empty]=data_[parent];
      empty=parent;
    }
    else{
      found=true;
    }
  }
  data_[empty]=curr;
  sz_++;
}

template <class T>
T MinHeap<T>::remove(){
  int last=sz_-1;
  T rc=data_[0];
  bool found=false;
  int empty=0;  //location of empty spot
  int left;     //index of left child of empty spot
  int right;    //index of right child of empty spot
  T curr=data_[last];  //the item that we need to find a new spot for in the MinHeap
  left=empty*2+1;    //calculate the index of the left child
  while(!found && left <= last){
    right=left+1;
    if(right > last){   //only a left child
      if(curr < data_[left])
        found=true;
      else{
        data_[empty]=data_[left];
        empty=left;
        left=empty*2+1;         
      }       
    } //if only left child
    else{
      if(curr < data_[left] && curr < data_[right]){
        found=true;
      }//if curr is bigger than children
      else{
        if(data_[left] <= data_[right]){
          data_[empty]=data_[left];
          empty=left;
          left=empty*2+1;
        }
        else{
          data_[empty]=data_[right];
          empty=right;
          left=empty*2+1;            
        }
      }//else not found
    }//else two children
  }//while
  sz_--;
  data_[empty]=curr;
  return rc;
}
