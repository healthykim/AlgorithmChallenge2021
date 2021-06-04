/**
 * @file graph.cc
 *
 */

#include <stack>
#include <queue>
#include <algorithm>
#include "graph.h"

namespace {
std::vector<Label> transferred_label;
void TransferLabel(const std::string &filename) {
    //initialize transferred_label array by order(tranferred_label[smallest label] <- 0)
  std::ifstream fin(filename);

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;
  int32_t graph_id_;
  size_t num_vertices_;

  std::set<Label> label_set;

  fin >> type >> graph_id_ >> num_vertices_;

  // preprocessing
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      label_set.insert(l);
    } else if (type == 'e') {
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;
    }
  }

  fin.close();

  transferred_label.resize(
      *std::max_element(label_set.begin(), label_set.end()) + 1, -1);

  Label new_label = 0;
  for (Label l : label_set) {
    transferred_label[l] = new_label;
    new_label += 1;
  }
}
}  // namespace

Graph::Graph(const std::string &filename, bool is_query) {

    if (!is_query) {
        //if !is_query, transferred_label array was not initialized.
    TransferLabel(filename);
    }
    std::vector<std::vector<Vertex>> adj_list;

  // Load Graph
  std::ifstream fin(filename);
  std::set<Label> label_set;

  if (!fin.is_open()) {
    std::cout << "Graph file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;

  fin >> type >> graph_id_ >> num_vertices_;

  adj_list.resize(num_vertices_);

  start_offset_.resize(num_vertices_ + 1);
  label_.resize(num_vertices_);

  num_edges_ = 0;

  // preprocessing
  while (fin >> type) {
    if (type == 'v') {
      Vertex id;
      Label l;
      fin >> id >> l;

      if (static_cast<size_t>(l) >= transferred_label.size())
        l = -1;
      else
        l = transferred_label[l];

      label_[id] = l;
      label_set.insert(l);
    } else if (type == 'e') {
        //labeling by vertex, not by edge. I think this code discard edge label
      Vertex v1, v2;
      Label l;
      fin >> v1 >> v2 >> l;

      adj_list[v1].push_back(v2);
      adj_list[v2].push_back(v1);

      num_edges_ += 1;
    }
  }


  fin.close();

  adj_array_.resize(num_edges_ * 2);

  num_labels_ = label_set.size();

  max_label_ = *std::max_element(label_set.begin(), label_set.end());

  label_frequency_.resize(max_label_ + 1);

  start_offset_by_label_.resize(num_vertices_ * (max_label_ + 1));

  start_offset_[0] = 0;

  for (size_t i = 0; i < adj_list.size(); ++i) {
      //initialize start_offset_ by start index where i's adj_vertex is saved
      //vertex 0's adj_vertex id is saved at adj_array_[start_offset[id]]~adj_array[start_offset[id+1]]
    start_offset_[i + 1] = start_offset_[i] + adj_list[i].size();
  }

  for (size_t i = 0; i < adj_list.size(); ++i) {
    label_frequency_[GetLabel(i)] += 1;

    auto &neighbors = adj_list[i];

    if (neighbors.size() == 0) continue;

    // sort neighbors by ascending order of label first, and descending order of
    // degree second
    std::sort(neighbors.begin(), neighbors.end(), [this](Vertex u, Vertex v) {
      if (GetLabel(u) != GetLabel(v))
        return GetLabel(u) < GetLabel(v);
      else if (GetDegree(u) != GetDegree(v))
        return GetDegree(u) > GetDegree(v);
      else
        return u < v;
    });

    Vertex v = neighbors[0];
    Label l = GetLabel(v);

    start_offset_by_label_[i * (max_label_ + 1) + l].first = start_offset_[i];

    for (size_t j = 1; j < neighbors.size(); ++j) {
      v = neighbors[j];
      Label next_l = GetLabel(v);

      if (l != next_l) {
        start_offset_by_label_[i * (max_label_ + 1) + l].second =
            start_offset_[i] + j;
        start_offset_by_label_[i * (max_label_ + 1) + next_l].first =
            start_offset_[i] + j;
        l = next_l;
      }
    }

    start_offset_by_label_[i * (max_label_ + 1) + l].second =
        start_offset_[i + 1];

    std::copy(adj_list[i].begin(), adj_list[i].end(),
              adj_array_.begin() + start_offset_[i]);
  }
}

/*
 * constructor for DAG-graph.
 * dag.cc overrides this
 */
Graph::Graph(const std::string &filename, const CandidateSet &candidateSet, bool is_query) {
    if (!is_query) {
        //if !is_query, transferred_label array was not initialized.
        TransferLabel(filename);
    }

    std::vector<std::pair<double, Vertex>> priority;

    // Load Graph
    std::ifstream fin(filename);
    std::set<Label> label_set;

    if (!fin.is_open()) {
        std::cout << "Graph file " << filename << " not found!\n";
        exit(EXIT_FAILURE);
    }

    char type;

    fin >> type >> graph_id_ >> num_vertices_;

    dag_adj.resize(num_vertices_);
    parents.resize(num_vertices_);

    start_offset_.resize(num_vertices_ + 1);
    label_.resize(num_vertices_);

    num_edges_ = 0;

    // preprocessing
    while (fin >> type) {
        if (type == 'v') {
            Vertex id;
            Label l;
            fin >> id >> l;

            if (static_cast<size_t>(l) >= transferred_label.size())
                l = -1;
            else
                l = transferred_label[l];

            label_[id] = l;
            label_set.insert(l);
        } else if (type == 'e') {
            //labeling by vertex, not by edge. I think this code discard edge label
            Vertex v1, v2;
            Label l;
            fin >> v1 >> v2 >> l;

            dag_adj[v1].push_back(v2);
            dag_adj[v2].push_back(v1);

            num_edges_ += 2;
        }
    }


    int mark[num_vertices_];
    for (Vertex v = 0; v < num_vertices_; v++) {
        double p = (double) candidateSet.GetCandidateSize(v) / dag_adj[v].size();
        priority.push_back(std::pair<double, Vertex>(p, v));
        mark[v] = 0;
    }

     Vertex v = std::min_element(priority.begin(), priority.end())->second;
     root = v;

    std::queue<Vertex> q;
     q.push(v);
     mark[v] = 1;
     while (!q.empty()) {
         /*make dag using backtracking*/
         v = q.front();
         std::vector<Vertex> neighbor = dag_adj[v];
         if(neighbor.empty()) {
             q.pop();
             continue;
         }
         std::sort(neighbor.begin(), neighbor.end(), [priority](Vertex x, Vertex u){
             return (priority[x] < priority[u]);
         });
         for(int i =0; i<neighbor.size(); i++)
         {
             std::vector<Vertex> new_adj;
             std::vector<Vertex> neighbor_neighbor = dag_adj[neighbor[i]];
             for(int j =0; j<neighbor_neighbor.size(); j++) {
                 if (neighbor_neighbor[j]!=v) {
                     new_adj.push_back(neighbor_neighbor[j]);
                 } else {
                     parents[neighbor[i]].push_back(neighbor_neighbor[j]);
                     num_edges_--;
                 }
             }
             dag_adj[neighbor[i]] = new_adj;
             if(!mark[neighbor[i]]) {
                 q.push(neighbor[i]);
                 mark[neighbor[i]]=1;
             }
         }
         q.pop();
     }


    fin.close();
//    for(int i=0; i<dag_adj.size(); i++){
//        std::cout<<"parent-child check: "<<i<<std::endl;
//        for(size_t j=0; j<parents[i].size(); j++) {
//            if(std::find(dag_adj[parents[i][j]].begin(), dag_adj[parents[i][j]].end(), i)==dag_adj[parents[i][j]].end())
//                std::cout<<"err"<<std::endl;
//        }
//        for(size_t j=0; j<dag_adj[i].size(); j++) {
//            if(std::find(parents[dag_adj[i][j]].begin(), parents[dag_adj[i][j]].end(), i)==parents[dag_adj[i][j]].end())
//                std::cout<<"err"<<std::endl;
//        }
//        std::cout<<std::endl;
//    }

    adj_array_.resize(num_edges_);

    num_labels_ = label_set.size();

    max_label_ = *std::max_element(label_set.begin(), label_set.end());

    label_frequency_.resize(max_label_ + 1);

    start_offset_by_label_.resize(num_vertices_ * (max_label_ + 1));

    start_offset_[0] = 0;

    for (size_t i = 0; i < dag_adj.size(); ++i) {
        //initialize start_offset_ by start index where i's adj_vertex is saved
        //vertex 0's adj_vertex id is saved at adj_array_[start_offset[id]]~adj_array[start_offset[id+1]]
        start_offset_[i + 1] = start_offset_[i] + dag_adj[i].size();
    }

    for (size_t i = 0; i < dag_adj.size(); ++i) {
        label_frequency_[GetLabel(i)] += 1;

        auto &neighbors = dag_adj[i];

        if (neighbors.size() == 0) continue;

        // sort neighbors by ascending order of label first, and descending order of
        // degree second
        std::sort(neighbors.begin(), neighbors.end(), [this](Vertex u, Vertex v) {
            if (GetLabel(u) != GetLabel(v))
                return GetLabel(u) < GetLabel(v);
            else if (GetDegree(u) != GetDegree(v))
                return GetDegree(u) > GetDegree(v);
            else
                return u < v;
        });

        Vertex v = neighbors[0];
        Label l = GetLabel(v);

        start_offset_by_label_[i * (max_label_ + 1) + l].first = start_offset_[i];

        for (size_t j = 1; j < neighbors.size(); ++j) {
            v = neighbors[j];
            Label next_l = GetLabel(v);

            if (l != next_l) {
                start_offset_by_label_[i * (max_label_ + 1) + l].second =
                        start_offset_[i] + j;
                start_offset_by_label_[i * (max_label_ + 1) + next_l].first =
                        start_offset_[i] + j;
                l = next_l;
            }
        }

        start_offset_by_label_[i * (max_label_ + 1) + l].second =
                start_offset_[i + 1];

        std::copy(dag_adj[i].begin(), dag_adj[i].end(),
                  adj_array_.begin() + start_offset_[i]);
    }
}

Graph::~Graph() {}
