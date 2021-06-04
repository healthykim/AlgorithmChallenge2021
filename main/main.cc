/**
 * @file main.cc
 *
 */

#include "backtrack.h"
#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include "dag.h"
#include <stdio.h>
int main(int argc, char* argv[]) {
 if (argc < 4) {
    std::cerr << "Usage: ./program <data graph file> <query graph file> "
                 "<candidate set file>\n";
    return EXIT_FAILURE;
 }

  std::string data_file_name = argv[1];
  std::string query_file_name = argv[2];
  std::string candidate_set_file_name = argv[3];

  Graph data(data_file_name);
  //printf("Graph ");
  CandidateSet candidate_set(candidate_set_file_name);
  //printf("Candidate ");
  Dag query(query_file_name, candidate_set, true);
//  std::cout<<query.GetNumEdges()<<std::endl;
//  std::cout<<"root "<<query.root<<std::endl;
//  for(int i=0; i<query.dag_adj.size(); i++){
//      std::cout<<"for "<<i<<std::endl;
//      for(size_t j=0; j<query.GetParentSize(i); j++) {
//          std::cout<<query.GetParent(i, j)<<" ";
//      }
//      std::cout<<std::endl;
//  }
//  for(int i=0; i<query.dag_adj.size(); i++){
//      std::cout<<"for "<<i<<std::endl;
//      for(size_t j=0; j<query.GetChildSize(i); j++) {
//          std::cout << query.GetChild(i,j) << " ";
//      }
//      std::cout<<std::endl;
//  }

  
  Backtrack backtrack(data, query, candidate_set);

  backtrack.PrintAllMatches();

  return EXIT_SUCCESS;
}
