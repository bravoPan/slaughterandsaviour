#ifndef PZQGAME_DISJOINTSET_H
#define PZQGAME_DISJOINTSET_H

struct DisjointSet{
  int * data;
  int * rank;
  int * nextData;
  int size;
  
  DisjointSet(int n);
  ~DisjointSet();
  void Init();
  int Find(int n);
  void Union(int a,int b);
  void Copy(DisjointSet &rhs);
};

#endif
