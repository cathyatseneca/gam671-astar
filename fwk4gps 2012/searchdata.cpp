/**********************************************************/
/*                                                        */
/* Author: Catherine Leung                                */
/*  */
/*  */
/**********************************************************/
#include "searchdata.h"
bool operator<(const SearchData& left, const SearchData& right){
  bool rc=false;
  if(left.cost() < right.cost())
    rc=true;
  return rc;
}
bool operator>(const SearchData& left, const SearchData& right){
  bool rc=false;
  if(left.cost() > right.cost())
    rc=true;
  return rc;
}
bool operator<=(const SearchData& left, const SearchData& right){
  bool rc=false;
  if(left.cost() <= right.cost())
    rc=true;
  return rc;
}
bool operator>=(const SearchData& left, const SearchData& right){
  bool rc=false;
  if(left.cost() >= right.cost())
    rc=true;
  return rc;
}