//
// Created by 문보설 on 2021/05/25.
//

#ifndef DAG_H
#define DAG_H
#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Dag : public Graph{
public:
    explicit Dag(const std::string& filename, const CandidateSet &candidateSet, bool is_query = false);
    inline size_t GetParentSize(Vertex v) const;
    inline size_t GetChildSize(Vertex v) const;
    inline size_t GetParent(Vertex v, size_t i) const;
    inline size_t GetChild(Vertex v, size_t) const;
    inline Vertex GetRoot() const;
    ~Dag();
};

inline Vertex Dag::GetRoot() const {
    return root;
}

inline size_t Dag::GetParent(Vertex v, size_t i) const {
    return parents[v][i];
}

inline size_t Dag::GetChild(Vertex v, size_t i) const {
    return dag_adj[v][i];
}

inline size_t Dag::GetChildSize(Vertex v) const {
    return dag_adj[v].size();
}

inline size_t Dag::GetParentSize(Vertex v) const {
    return parents[v].size();
}


#endif //DAG_H
