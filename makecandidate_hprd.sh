for i in 'n' 's'
do
for j in 1 3 5 8
do
    ./executable/filter_vertices data/lcc_hprd.igraph query/lcc_hprd_$i$j.igraph >> candidate/lcc_hprd_c$i$j.igraph
done
done

