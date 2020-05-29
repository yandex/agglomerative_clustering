cmake_minimum_required(VERSION 3.10)
project(agglomerative_clustering_lib)
add_library(agglomerative_clustering_lib STATIC clustering.cpp dsu.cpp elements.cpp)
