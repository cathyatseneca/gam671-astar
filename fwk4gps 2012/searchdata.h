/**********************************************************/
/*                                                        */
/* Author: Catherine Leung                                */
/*  */
/*  */
/**********************************************************/
#ifndef SEARCHDATA_H
#define SEARCHDATA_H
class SearchData{
  int nodenum_;          //node number
  int parent_;           //where the node came from
  float cost_;           //"cost" this cost changes depending on type of search
                         //being conducted
public:
  void set(int nodenum,int parent,float cost){
    nodenum_=nodenum;
    parent_=parent;
    cost_=cost;
  }
  float cost() const{return cost_;}
  int nodenum() const{return nodenum_;}
  int parent() const{return parent_;}

};/*struct*/
bool operator<(const SearchData& left, const SearchData& right);
bool operator>(const SearchData& left, const SearchData& right);
bool operator<=(const SearchData& left, const SearchData& right);
bool operator>=(const SearchData& left, const SearchData& right);
#endif