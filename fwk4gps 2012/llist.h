#ifndef LLIST_H
#define LLIST_H
#define NULL 0
template <class T>
class LList;

template <class T>
class Node{
  friend LList<T>;
  T data_;
  Node<T>* next_;
  Node<T>* prev_;
public:
  Node(const T& data){
    data_=data;
    next_=NULL;
    prev_=NULL;
  }
  const Node<T>* next() const{return next_;}
  const Node<T>* prev() const{return prev_;}
  const T& data() const{return data_;} 

};/*class*/

template <class T>
class LList{
  Node<T>* start_;
  Node<T>* curr_;  //internal pointer
public:
  LList();
  ~LList();
  LList(const LList<T>& other);
  const LList<T>& operator=(const LList<T>& other);
  void insert(const T& data);
  bool isempty() const{return start_==NULL;}
  bool get(T passback[], int max) const;
  Node<T>* find(const void* key, bool (*match)(const void*,const T&));
  bool remove(const void* key, bool (*match)(const void*,const T&));
  void gostart(){curr_=start_;}
  Node<T>* curr(){return curr_;}
  void gonext(){curr_=curr_->next_;}
};/*class*/

template <class T>
LList<T>::LList(){
  start_=NULL;
  curr_=NULL;
}
template <class T>
LList<T>::~LList(){
  curr_=start_;
  Node<T>* rm;
  while(curr_){
    rm=curr_;
    curr_=curr_->next_;
    delete rm;
  }
}
template <class T>
LList<T>::LList(const LList<T>& other){
  const Node<T>* curr=other.start_;
  start_=NULL;
  curr_=NULL;
  if(curr){
    /*go to back*/
    while(curr->next())
      curr=curr->next();
    /*call insert*/
    while(curr){
      insert(curr->data());
      if(curr==other.curr_)  //insert always puts node at front
        curr_=start_;       
      curr=curr->prev();
    }
  }//if
}

template <class T>
const LList<T>& LList<T>::operator=(const LList<T>& other){
  if(this !=&other){
    Node<T>* curr=start_;
    Node<T>* rm;
    while(curr){
      rm=curr;
      curr=curr->next_;
      delete rm;
    }
    start_=NULL;
    curr=other.start_;
    if(curr){
      while(curr->next_)
        curr=curr->next_;
      while(curr){
        insert(curr->data());
        if(curr==other.curr_)  //insert always puts node at front
          curr_=start_;       
        curr=curr->prev_;
      } 
    }//if
  }
  return *this;
}

template <class T>
void LList<T>::insert(const T& data){
  Node<T>* nn=new Node<T>(data);
  nn->next_=start_;
  nn->prev_=NULL;
  if(start_)
    start_->prev_=nn;
  start_=nn;
  curr_=nn;
}

template <class T>
bool LList<T>::get(T passback[], int max) const{
  const Node<T>* curr=start_;
  int i=0;
  bool rc;
  while(curr && i<max){
    passback[i]=curr->data();
    i++;
    curr=curr->next();
  }
  if(!curr)
    rc=true;
  return rc;
}
template <class T>
Node<T>* LList<T>::find(const void* key, bool (*match)(const void*,const T&)){
  Node<T>* curr=start_;
  Node<T>* rc=NULL;
  while(!rc && curr){
    if(match(key,curr->data())){
      curr_=rc=curr;
      
    }
    curr=curr->next_;
  }
  return rc;
}

template <class T>
bool LList<T>::remove(const void* key, bool (*match)(const void*,const T&)){
  Node<T>* rm=find(key,match);
  Node<T>* p;
  Node<T>* n;
  bool rc=false;
  if (rm){
    if(curr_==rm)
      curr_=rm->next_;
    p=rm->prev_;
    n=rm->next_;
    if(p){
      p->next_=n;
    }
    else{
      start_=n;
    }
    if(n)
      n->prev_=p;
    delete rm;
    rc=true;
  }/*if*/
  return rc;
}

#endif