#include "DisjointSet.h"

DisjointSet::DisjointSet(int n) : data(new int[n]),rank(new int[n]),nextData(new int[n]),size(n){
  for(int i = 0;i < n;++i){
    data[i] = i;
    rank[i] = 1;
    nextData[i] = -1;
  }
}

void DisjointSet::Init(){
  for(int i = 0;i < size;++i){
    data[i] = i;
    rank[i] = 1;
    nextData[i] = -1;
  }
}

int DisjointSet::Find(int n){
  while(data[n] != n){
    data[n] = data[data[n]];
    n = data[n];
  }
  return n;
}

void DisjointSet::Union(int a,int b){
  int p = Find(a),q = Find(b);
  if(p == q){return;} else if(p > q){
    data[q] = p;
    if(rank[p] == rank[q]){++rank[p];}
  } else {
    data[p] = q;
    if(rank[p] == rank[q]){++rank[q];}
  }
}

void DisjointSet::Copy(DisjointSet &rhs){
  for(int i = 0;i < size;++i){
    data[i] = rhs.data[i];
    rank[i] = rhs.rank[i];
    nextData[i] = -1;
  }
}

DisjointSet::~DisjointSet(){
  delete[] data;
  delete[] rank;
  delete[] nextData;
}
