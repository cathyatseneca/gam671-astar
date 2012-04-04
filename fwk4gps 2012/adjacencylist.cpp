#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <cstdio>
#include "GeneralConstants.h"  // for speed constants
#include "llist.h"
#include "adjacencylist.h"


AdjacencyList::AdjacencyList(){
  cap_=20;
  vertexlist_=new VInfo*[20];
  size_=0;
  numedges_=0;
}
AdjacencyList::AdjacencyList(const char* fname){
  FILE* graphdata=fopen(fname,"r");


  int numvert;
  Vector pos;
  int numedge;
  int v1,v2;
  float speed;
  char speedchar;  //the character read in from the file,
                   //A is fast, B is medium, C is slows
                   //actual speeds stored as global constants in general.h
                   //you can modify general.h to change the actual speeds
                   //to show your results better
//  debug=fopen("aldebug.txt","w");

  fscanf(graphdata,"%d\n",&numvert);
  cap_=numvert;
  vertexlist_=new VInfo*[cap_];
  size_=0;
  for(int i=0;i<numvert;i++){
    fscanf(graphdata,"%f %f %f\n",&(pos.x),&(pos.y),&(pos.z));
    AddVert(pos);
  }
  fscanf(graphdata,"%d\n",&numedge);
  for(int i=0;i<numedge;i++){
    fscanf(graphdata,"%d %d %c\n",&v1,&v2,&speedchar);
    switch(speedchar){
      case 'A': speed=FAST; break;
      case 'B': speed=MEDIUM; break;
      case 'C': speed=SLOW; break;
    }
    AddEdge(v1,v2,speed);
  }
  numedges_=numedge;
  fclose(graphdata);
}
void AdjacencyList::AddVert(const Vector& pos){
  if(size_ >=cap_){  //grow the list
    cap_+=10;
    VInfo** tmp=new VInfo*[cap_];
    for(int i=0;i<size_;i++){
      tmp[i]=vertexlist_[i];
    }
    delete vertexlist_;
    vertexlist_=tmp;
  }
  vertexlist_[size_]=new VInfo(pos,size_);
  size_++;
}
bool AdjacencyList::AddEdge(int v1,int v2, float speed){
  bool rc=false;
  if(v1 < size_ && v2 <size_){
    rc=true;
    vertexlist_[v1]->AddConnection(v2,speed);
    vertexlist_[v2]->AddConnection(v1,speed);
  }
  return rc;
}
bool AdjacencyList::AddUniEdge(int from,int to,float speed){
  bool rc=false;
  if(from < size_ && to <size_){
    rc=true;
    vertexlist_[from]->AddConnection(to,speed);
  }
  return rc;
}
float AdjacencyList::speed(int fromvert, int tovert){
  float rc=0;
  if(fromvert < size_ && tovert < size_){
    LList<EdgeInfo>& edgelist=edges(fromvert);
    Node<EdgeInfo>* curr;
    bool done=false;
    while(curr=edgelist.curr()){
      if(curr->data().to_==tovert){
        rc=curr->data().speed_;
        done=true;
      }
 //     fprintf(debug,"curr: %d\n",curr);
 //     fprintf(debug,"to: %d\n",curr->data().to_);
      edgelist.gonext();
    }
  }
  return rc;
}

AdjacencyList::~AdjacencyList(){
  for(int i=0;i<size_;i++){
    delete vertexlist_[i];
  }
  delete [] vertexlist_;
}
