/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"
using namespace std;

class Backtrack {
 public:
  Backtrack(const Graph &d, const Graph &q, const CandidateSet &c);
  ~Backtrack();

  void PrintAllMatches();

 private:
 void Backtrack::backtrack(Vertex curr);
 bool Backtrack::check_candidate(Vertex curr, Vertex curr_cs, const vector<Vertex> &curr_parent);
 void Backtrack::printembedding();
 void Backtrack::update_extendable(Vertex curr);

 size_t cnt; /*# of embedding got*/

 vector<Vertex> embedding; /*partial embedding, embedding(u) = v, u: vertex of query, v: vertex of data*/
 size_t embedding_size; /*# of vertices in partial embedding*/
 size_t q_size; /*# of vertices of query graph*/

 /*array of extendable vertices: <if extendable, array of extendable candidates>*/
 vector<pair<bool,vector<Vertex>>> extendable;

 const Graph &data;
 const Graph &query;
 const CandidateSet &cs;

 Vertex root; /*root of query DAG*/

};

#endif  // BACKTRACK_H_
