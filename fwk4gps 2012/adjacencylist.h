
#ifndef _ADJACENCYLIST_H_
#define _ADJACENCYLIST_H_

#include "llist.h"
#include "MathDeclarations.h"
#include "searchdata.h"
#include <cstdio>


/*In the comments below, the term "vertex" refers to the graph's vertex and not
  a vertex within its model*/

/*this struct contains the information about each connection.  where it is
  from, where it goes to. It also contains the rate at which an object can
  travel between the two vertex (velocity)*/
class VInfo;
class AdjacencyList;

class EdgeInfo{
  friend VInfo;
  friend AdjacencyList;
  int to_;       //vertex number of the vertex edge leads to (index in adjacency list)
  int from_;     //vertex number of vertex edge came from  (index in adjacency list)
  float speed_;  //in metres per second
public:
  int from() const{return from_;}
  int to()const{return to_;}
  float speed()const{return speed_;}
  void set(int from,int to,float speed){
    from_=from;
    to_=to;
    speed_=speed;
  }
};/*EdgeInfo*/

/*A vertex informational struct.  contains the vertex's position
  and a list of all the vertices it is connected to*/
class VInfo{
  friend AdjacencyList;
	Vector pos_;                  // position of the frame wrt its parent frame
  LList<EdgeInfo> connection_;  //linked list of vertices this vertex is connected to
  int vertnum_;                 //its vertex number in the adjacency list
public:
  VInfo(const Vector& pos,int vertnum){
    pos_=pos;
    vertnum_=vertnum;

  }
  VInfo(int vertnum):pos_(0,0,0){
    vertnum_=vertnum;
  }
  void AddConnection(int to, float speed){
    EdgeInfo newedge;
    newedge.set(vertnum_,to,speed);
    connection_.insert(newedge);
  }
};/*struct*/

class AdjacencyList{
  VInfo** vertexlist_;     //array of vertex pointers.  Using pointers for
                           //three reasons: 1) less memory usage, 2)
                           //when growing vertex list, less copying is needed
                           //3) no default constructor to VInfo

  int cap_;               //current array capacity
  int size_;              //size of array (number of elements used)
  int numedges_;		  //number of edges
//  FILE* debug;
public:
  AdjacencyList();                  //graph is empty
  AdjacencyList(const char* fname); //constructed with a file whose name is fname
                                    //see comments at bottom of this file for format
  void AddVert(const Vector& pos);
  bool AddEdge(int v1,int v2, float speed);  //adds a bi-directional link between v1 and v2
  bool AddUniEdge(int from,int to, float speed);  //adds a unidirctional link between from 
                                                  //and to
  int numvert(){return size_;}
  int numedges(){return numedges_;}
  float vx(int vnum){return vertexlist_[vnum]->pos_.x;} 
  float vy(int vnum){return vertexlist_[vnum]->pos_.y;} 
  float vz(int vnum){return vertexlist_[vnum]->pos_.z;}
  Vector pos(int vnum){return vertexlist_[vnum]->pos_;}
  LList<EdgeInfo>& edges(int vnum){
    vertexlist_[vnum]->connection_.gostart();
    return vertexlist_[vnum]->connection_;
  }
  float speed(int fromvert,int tovert);  //given two vertex, returns the rate of travel
                                         //between them, 0 if no edge between them
  ~AdjacencyList();
};/*class*/
/*File format for initializing an AdjacencyList must follow the following strictly:
Let N represent number of vertices
Let E represent number of edges
All numbers are space separated
All lines are terminated with a newline

first line: single number representing number of vertices
next N lines: position of each vertex
next line: single number representing number of edges
next E lines: fromvertex tovertex speed   (note all vertices added bidirectionally)
*/
struct CloseStructure{
  bool closed_;          //true if node has been examined, false otherwise
  SearchData nodeinf_;
  CloseStructure(){closed_=false;}
};/*struct*/

#endif