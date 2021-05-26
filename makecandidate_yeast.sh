for i in 'n' 's'
do
for j in 1 3 5 8
do
    ./executable/filter_vertices data/lcc_yeast.igraph query/lcc_yeast_$i$j.igraph >> candidate/lcc_yeast_c$i$j.igraph
done
done

