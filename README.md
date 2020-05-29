# Agglomerative Clustering

Fast agglomerative graph clustering method
Details: https://habr.com/ru/company/yandex/blog/503492/

**1. Build**

```
git clone https://github.com/yandex/agglomerative_clustering/ .
cmake .
cmake --build .
```

**2. Run**

```
./agglomerative_clustering -f 10 < data/2d_similarities.tsv > 2d_clusters.tsv
../cluster_metrics/cluster_metrics data/2d_markup.tsv 2d_clusters.tsv
ECC   0.62411 (0.62411)
BCP   0.74112 (0.74112)
BCR   0.68095 (0.68095)
BCF1  0.70976 (0.70976)
```

Here we're using ```cluster_metrics``` program from https://github.com/yandex/cluster_metrics/.
Sample data represents 18343 points generated by data/2d_gen.py:

![](https://user-images.githubusercontent.com/6789687/82741211-8eff4b80-9d58-11ea-8d0b-da1f552c98c4.png)

2/3 of connections are eliminated randomly to demonstrate that algorithm is resistant in case of information losses.