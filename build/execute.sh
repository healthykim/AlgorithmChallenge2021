for k in 'hprd' 'human' 'yeast'
do
for i in 'n' 's'
do
for j in 1 3 5 8
do
./main/program ../data/lcc_$k.igraph ../query/lcc_$k\_$i$j.igraph ../candidate/lcc_$k\_c$i$j.igraph
done
done
done