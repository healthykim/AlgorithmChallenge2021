/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <queue>
#include <stdio.h>
using namespace std;


Backtrack::Backtrack(const Graph &d, const Dag &q, const CandidateSet &c): data(d), query(q), cs(c){
  
  cnt = 0;
  q_size = query.GetNumVertices();
  embedding = vector<Vertex>(q_size, -1);  
  embedding_size = 0;  
  pair<size_t, vector<Vertex>> init = make_pair(0, vector<Vertex>());
  extendable = vector<pair<size_t,vector<Vertex>>>(q_size, init);


  root = query.GetRoot();
}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches() {
  printf("t %lu\n", query.GetNumVertices());
  // implement your code here.
  backtrack(root); 
}

void  Backtrack::printembedding(){
 
  /*for checking*/
  //if(check()!=0) printf("wrong embedding: %d    ", check());

  printf("a ");

  for(size_t i=0; i<q_size; i++){
    printf("%d ", embedding[i]);
  }
  printf("\n");
}

int Backtrack::check(){
  for(size_t i=0; i<q_size; i++){
    /*check condition 2: same label*/
    if(query.GetLabel(i)!=data.GetLabel(embedding[i])) return 2;

    for(size_t j=i+1; j<q_size; j++){
      /*check condition 1: injectivity*/
      if(embedding[i]==embedding[j]) return embedding[i];

      /*check condition 3: edges*/
      if(query.IsNeighbor(i,j)) if(!data.IsNeighbor(embedding[i], embedding[j])) return 3;
    }
 

  }
  return 0;
}

void Backtrack::backtrack(Vertex curr){
  size_t curr_cs_size; /*candidate space size for curr vertex*/

  curr_cs_size = cs.GetCandidateSize(curr);

  if(curr==root){
      /*map curr vertex to candidate space*/
      for(size_t i =0; i<curr_cs_size; i++){
        Vertex curr_cs = cs.GetCandidate(curr, i); /*candidate for mapping*/

        /*injectivity & parent edge condition need not to be checked for root,
        so directly map and update partial embedding*/
        embedding[curr] = curr_cs;
        embedding_size++;

        if(embedding_size==q_size){ /*if embedding is found*/
          cnt++;
          printembedding();
          if(cnt>=100000) return;

        }
        else{
          /*Candidate-size order for bactracking*/

          /*update the list of extendable vertices due to the update of partial embedding */
          /*extendable candidates are also checked in this function*/
          update_extendable(curr);

          
          //priority_queue<extendable_pair, vector<extendable_pair>, greater<extendable_pair>> pq;

          /*make min heap of the extendable vertices based on the # of extendable candidates*/
          /*for(size_t j=0; j<q_size; j++){
            if(extendable[j].first==0||embedding[j]!=-1) continue;
            else pq.push(make_pair(extendable[j], j));
          }
          */
          /*visiting each of them in candidate size order*/
          /*
          while(!pq.empty()){
            backtrack(pq.top().second);
            pq.pop();
            if(cnt>=100000) return;
          }
          */

          size_t min= SIZE_MAX;
          int min_index=-1;

          for(size_t j=0; j<q_size; j++){
            if(embedding[j]!=-1||extendable[j].first==0) continue;
            else if(extendable[j].first<min){
                min = extendable[j].first;
                min_index = j;
              }
          }

          if(min_index!=-1) backtrack(min_index);
          
        }       
        /*in order to search other candidate for same vertex*/
        embedding_size--;
        embedding[curr] = -1;
      
             
      }

      return;
  }
  else{
      vector<Vertex> curr_cs_candidate = extendable[curr].second;

       /*checking for candidate is already done in update_extendable of previous level
       so we can freely add every vertices in extendabe[curr].second to embedding*/
      for(Vertex curr_cs: curr_cs_candidate){
       
        if(embedding.end()!=find(embedding.begin(), embedding.end(), curr_cs)) continue;

        embedding[curr] = curr_cs; /*map and add to partial embedding*/
        embedding_size++;

        if(embedding_size==q_size){ /*if embedding is found*/
          cnt++;
          printembedding();
          if(cnt>=100000) return;
        }
        else{
          /*same as above*/
          update_extendable(curr);

          //priority_queue<extendable_pair, vector<extendable_pair>, greater<extendable_pair>> pq;
          /*
          for(size_t j=0; j<q_size; j++){
            if(embedding[j]!=-1||extendable[j].first==0) continue;
            else pq.push(make_pair(extendable[j], j));
          }
               
          while(!pq.empty()){
            backtrack(pq.top().second);
            pq.pop();
            if(cnt>=100000) return;
          }
          */

          size_t min= SIZE_MAX;
          int min_index=-1;

          for(size_t j=0; j<q_size; j++){
            if(embedding[j]!=-1||extendable[j].first==0) continue;
            else{ 
              size_t real_cs_size = extendable[j].first;

              for(Vertex cd: extendable[j].second){
                if(embedding.end()!=find(embedding.begin(), embedding.end(), cd)) real_cs_size--;
              }
              if(real_cs_size>0&&real_cs_size<min){
                min = real_cs_size;
                min_index = j;
              }
            }
          }

          if(min_index!=-1) backtrack(min_index);
        }       
        /*in order to search other candidate for same vertex*/
        embedding_size--;
        embedding[curr] = -1;
      }

      /*change extendable status before returning to previous stage*/
      update_extendable(curr);
      return;
  }  


}
    

/*check if the vertex of cs is an appropriate candidate of mapping for curr*/
bool Backtrack::check_candidate(Vertex curr, Vertex curr_cs, const vector<Vertex> &curr_parent){   

    /*check injectivity*/
    if(embedding.end()!=find(embedding.begin(), embedding.end(), curr_cs)) return false;
            
    /*check if edges with parents exist*/
    bool edge_exist = true;
    for(Vertex parent: curr_parent){
      if(embedding[parent]==-1) continue; /*parent not yet in embedding*/
      else{
        if(!data.IsNeighbor(embedding[parent], curr_cs)){ /*if no edge exist*/
          edge_exist = false;
          break;
        }
      }
    }
    return edge_exist;
}

void Backtrack::update_extendable(Vertex curr){
   
   size_t curr_child_size = query.GetChildSize(curr);

   for(size_t i=0; i<curr_child_size; i++){ /*check if child is extendable vertex*/
    Vertex child = query.GetChild(curr, i);

    if(embedding[child]!=-1) continue;

    if(embedding[curr]==-1){
      extendable[child] = make_pair(0, vector<Vertex>());
    }
    else{
     vector<Vertex> parent_child;
     size_t parent_child_size = query.GetParentSize(child);
     bool if_extendable = true;

     for(size_t j=0; j<parent_child_size; j++){
       Vertex parent = query.GetParent(child, j);
       parent_child.push_back(parent);
       if(embedding[parent]==-1){
        if_extendable = false;
        break;
       }
     }

     if(if_extendable){ /*if extendable, find extendable candidates*/
        vector<Vertex> candidates;

        size_t child_cs_size = cs.GetCandidateSize(child);
        Vertex child_cs;

        for(size_t i =0; i<child_cs_size; i++){
          child_cs = cs.GetCandidate(child, i); /*candidate for mapping*/
          if(check_candidate(child, child_cs, parent_child)) candidates.push_back(child_cs);
        }
        extendable[child] = make_pair(candidates.size(), candidates);

     }
     else{
        extendable[child] = make_pair(0, vector<Vertex>());
     }
    }
  }
}
