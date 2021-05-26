/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include "dag.h"
using namespace std;

class Backtrack {
 public:
  Backtrack(const Graph &d, const Dag &q, const CandidateSet &c);
  ~Backtrack();

  void PrintAllMatches();

 private:
 void backtrack(Vertex curr);
 bool check_candidate(Vertex curr, Vertex curr_cs, const vector<Vertex> &curr_parent);
 void printembedding();
 void update_extendable(Vertex curr);

 size_t cnt; /*# of embedding got*/

 vector<Vertex> embedding; /*partial embedding, embedding(u) = v, u: vertex of query, v: vertex of data*/
 size_t embedding_size; /*# of vertices in partial embedding*/
 size_t q_size; /*# of vertices of query graph*/

 /*array of extendable vertices: <if extendable, array of extendable candidates>*/
 vector<pair<size_t,vector<Vertex>>> extendable;

 const Graph &data;
 const Dag &query;
 const CandidateSet &cs;

 Vertex root; /*root of query DAG*/

};

#endif  // BACKTRACK_H_
