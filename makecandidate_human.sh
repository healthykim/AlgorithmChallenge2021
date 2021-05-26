for i in 'n' 's'
do
for j in 1 3 5 8
do
    ./executable/filter_vertices data/lcc_human.igraph query/lcc_human_$i$j.igraph >> candidate/lcc_human_c$i$j.igraph
done
done

