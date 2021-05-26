/**
 * @file graph.cc
 *
 */

#include <stack>
#include <algorithm>
#include "graph.h"

namespace {
std::vector<Label> transferred_label;
void TransferLabel(const std::string &filename) {
    //initialize transferred_label array by order(tranferred_label[smallest label] <- 0)
    //i don't know why TA did this job.
    //possibility : given label value is too large
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
    dag_adj = std::vector<std::vector<Vertex>>();

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

Graph::Graph(const std::string &filename, const CandidateSet &candidateSet, bool is_query) {
    if (!is_query) {
        //if !is_query, transferred_label array was not initialized.
        TransferLabel(filename);
    }
   // std::vector<std::vector<Vertex>> adj_list;
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


     std::stack<Vertex> s;
     s.push(v);
     mark[v] = 1;
     while (!s.empty()) {
         v = s.top();
//         std::cout<<"for " << v << std::endl;
         std::vector<Vertex> neighbor = dag_adj[v];
         if(neighbor.empty()) {
             s.pop();
             continue;
         }
         Vertex nn = neighbor.front();
         for (int i = 1; i < neighbor.size(); i++) {
             if ((priority[nn] > priority[i]) && !mark[i])
                 nn = neighbor[i];
             else if (mark[nn]) {
                 nn = neighbor[i];
             }
         }
          if (mark[nn])
              s.pop();
          else {
//              std::cout << nn << std::endl;
              std::vector<Vertex> new_adj;
              for(int i =0; i<dag_adj[nn].size(); i++)
              {
                  if(!mark[dag_adj[nn][i]]) {
                      new_adj.push_back(dag_adj[nn][i]);
                  }
                  else
                      parents[nn].push_back(dag_adj[nn][i]);
              }
              dag_adj[nn] = new_adj;
              s.push(nn);
              mark[nn] = 1;
          }
     }
///FOR DEBUGGING
//     for(int i=0; i<parents.size(); i++){
//         std::cout<<"parent of "<< i <<std::endl;
//         for(int j=0; j<parents[i].size(); j++){
//             std::cout<< parents[i][j] <<" ";
//         }
//         std::cout<<std::endl;
//     }
//    for(int i=0; i<dag_adj.size(); i++){
//        std::cout<<"child of "<< i <<std::endl;
//        for(int j=0; j<dag_adj[i].size(); j++){
//            std::cout<< dag_adj[i][j] <<" ";
//        }
//        std::cout<<std::endl;
//    }

    fin.close();

//    adj_array_.resize(num_edges_);
//
//    num_labels_ = label_set.size();
//
//    max_label_ = *std::max_element(label_set.begin(), label_set.end());
//
//    label_frequency_.resize(max_label_ + 1);
//
//    start_offset_by_label_.resize(num_vertices_ * (max_label_ + 1));
//
//    start_offset_[0] = 0;
//
//    for (size_t i = 0; i < adj_list.size(); ++i) {
//        //initialize start_offset_ by start index where i's adj_vertex is saved
//        //vertex 0's adj_vertex id is saved at adj_array_[start_offset[id]]~adj_array[start_offset[id+1]]
//        start_offset_[i + 1] = start_offset_[i] + adj_list[i].size();
//    }
//
//    for (size_t i = 0; i < adj_list.size(); ++i) {
//        label_frequency_[GetLabel(i)] += 1;
//
//        auto &neighbors = adj_list[i];
//
//        if (neighbors.size() == 0) continue;
//
//        // sort neighbors by ascending order of label first, and descending order of
//        // degree second
//        std::sort(neighbors.begin(), neighbors.end(), [this](Vertex u, Vertex v) {
//            if (GetLabel(u) != GetLabel(v))
//                return GetLabel(u) < GetLabel(v);
//            else if (GetDegree(u) != GetDegree(v))
//                return GetDegree(u) > GetDegree(v);
//            else
//                return u < v;
//        });
//
//        Vertex v = neighbors[0];
//        Label l = GetLabel(v);
//
//        start_offset_by_label_[i * (max_label_ + 1) + l].first = start_offset_[i];
//
//        for (size_t j = 1; j < neighbors.size(); ++j) {
//            v = neighbors[j];
//            Label next_l = GetLabel(v);
//
//            if (l != next_l) {
//                start_offset_by_label_[i * (max_label_ + 1) + l].second =
//                        start_offset_[i] + j;
//                start_offset_by_label_[i * (max_label_ + 1) + next_l].first =
//                        start_offset_[i] + j;
//                l = next_l;
//            }
//        }
//
//        start_offset_by_label_[i * (max_label_ + 1) + l].second =
//                start_offset_[i + 1];
//
//        std::copy(adj_list[i].begin(), adj_list[i].end(),
//                  adj_array_.begin() + start_offset_[i]);
//    }
}

Graph::~Graph() {}
