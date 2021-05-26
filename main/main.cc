/**
 * @file main.cc
 *
 */

#include "backtrack.h"
#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include "dag.h"

int main(int argc, char* argv[]) {
//  if (argc < 4) {
//    std::cerr << "Usage: ./program <data graph file> <query graph file> "
//                 "<candidate set file>\n";
//    return EXIT_FAILURE;
//  }

  std::string data_file_name = "/Users/munboseol/AlgorithmChallenge2021/data/lcc_hprd.igraph";
  std::string query_file_name = "/Users/munboseol/AlgorithmChallenge2021/query/lcc_hprd_n1.igraph";
  std::string candidate_set_file_name = "/Users/munboseol/AlgorithmChallenge2021/candidate/lcc_hprd_cn1.igraph";

  Graph data(data_file_name);
  CandidateSet candidate_set(candidate_set_file_name);
  Dag query(query_file_name, candidate_set, true);


  Backtrack backtrack(data, query, candidate_set);

  backtrack.PrintAllMatches();

  return EXIT_SUCCESS;
}
