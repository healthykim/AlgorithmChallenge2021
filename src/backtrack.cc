/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <queue>
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
  
  // implement your code here.
  backtrack(root); 
}

void  Backtrack::printembedding(){
  cout<<"a ";
  for(size_t i=0; i<q_size; i++){
    cout<<embedding[i]<<" ";
  }
  cout<<"\n";
}

void Backtrack::backtrack(Vertex curr){
  size_t curr_cs_size; /*candidate space size for curr vertex*/   

  curr_cs_size = cs.GetCandidateSize(curr);

  if(curr==root){
      /*map curr vertex to candidate space*/
      for(size_t i =0; i<curr_cs_size; i++){
        Vertex curr_cs = cs.GetCandidate(curr, i); /*candidate for mapping*/

        /*As checking the condition is done, map curr with curr_cs*/
        embedding[curr] = curr_cs;
        embedding_size++;

        if(embedding_size==q_size){
          cnt++;
          printembedding();
          if(cnt>100000) return;

          /*in order to search other candidate for same vertex*/
          embedding_size--;
          embedding[curr] = -1;
          continue;
        }
        else{
          /*Candidate-size order*/
          update_extendable(curr);

          vector<pair<pair<size_t, vector<Vertex>>, Vertex>> real_extend; 

          for(size_t j=0; j<q_size; j++){
            if(extendable[j].first==0) continue;
            else real_extend.push_back(make_pair(extendable[j], j));
          }

          make_heap(real_extend.begin(), real_extend.end());
          for(int j=real_extend.size()-1; j>=0; j--){
              backtrack(real_extend[j].second);
            if(cnt>100000) return;
          }
          embedding[curr] = -1;
          embedding_size--;  
        }       
             
      }

      return;
  }
  else{
      vector<Vertex> curr_cs_candidate = extendable[curr].second;

      for(Vertex curr_cs: curr_cs_candidate){
        /*checking for candidate is already done in update_extendable of previous level*/
        embedding[curr] = curr_cs;
        embedding_size++;

        if(embedding_size==q_size){
          cnt++;
          printembedding();
          if(cnt>100000) return;

          /*in order to search other candidate for same vertex*/
          embedding_size--;
          embedding[curr] = -1;
          continue;
        }
        else{
          update_extendable(curr);

          typedef pair<pair<size_t, vector<Vertex>>, Vertex> extendable_pair;
          priority_queue<extendable_pair, vector<extendable_pair>, greater<extendable_pair>> pq;

          for(size_t j=0; j<q_size; j++){
            if(extendable[j].first==0) continue;
            else pq.push(make_pair(extendable[j], j));
          }
               
          while(!pq.empty()){
            backtrack(pq.top().second);
            pq.pop();
            if(cnt>100000) return;
          }
          embedding_size--;
          embedding[curr] = -1;

        }       
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

   for(size_t i=0; i<curr_child_size; i++){ /*check if child is extendable vertexd*/
    Vertex child = query.GetChild(curr, i);
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
