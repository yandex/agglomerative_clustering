import numpy as np
import math
import random
import re
import sys

maxSize = 500
powerParam = 0.6

left   = 0
bottom = 0
right  = 150
top    = 150

clusterSizes = []
for i in range(1, 1000):
    nextSize = max(maxSize / math.pow(i, powerParam), 1)
    clusterSizes.append(int(nextSize))

points = []
pointClusters = []

clusterCenters = []
for clusterIdx in range(len(clusterSizes)):

    center = [random.randint(left, right), random.randint(bottom, top)]

    for idx in range(clusterSizes[clusterIdx]):
        alpha = random.random() * math.pi * 2
        radius = np.random.normal()
        point = [math.sin(alpha) * radius + center[0], math.cos(alpha) * radius + center[1]]

        points.append(point)
        pointClusters.append(clusterIdx)

similarities = []
for i in range(len(points)):
    for j in range(i + 1, len(points)):
        diffX = points[i][0] - points[j][0]
        diffY = points[i][1] - points[j][1]

        if diffX > 5 or diffY > 5:
            continue

        distance = math.sqrt(diffX * diffX + diffY * diffY)
        if distance > 5:
            continue

        similarity = 2. / (1. + math.exp(distance * distance))

        if similarity < 1e-3:
            continue

        similarities.append([i, j, similarity])

    print >> sys.stderr, i, len(points)

random.shuffle(similarities)
similarities = similarities[: len(similarities) / 3]

markupOut = open("2d_markup.tsv", "w")
pointsOut = open("2d_points.tsv", "w")
simOut = open("2d_similarities.tsv", "w")

for similarityInfo in similarities:
    print >> simOut, "\t".join(map(str, similarityInfo))
for i in range(len(points)):
    print >> markupOut, "\t".join(map(str, [pointClusters[i], i]))
print >> pointsOut, "\n".join(map(lambda x : "\t".join(map(str, x)), points))

markupOut.close()
pointsOut.close()
simOut.close()
