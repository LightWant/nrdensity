import sys

fpath = sys.argv[1]

edges = []
minV = 10000000
maxV = 0

with open(fpath, 'r') as f:
    for l in f:
        if l[0] == '#' or l[0] == '%':
            continue
        vs = list(map(int, l.split()))
        u, v = vs[0], vs[1]
        if(u > v):
            t = u
            u = v
            v = t
        edges.append((u, v))
        minV = min(minV, u)
        maxV = max(maxV, v)

edges.sort()


m = 1
for i in range(1, len(edges)):
    if edges[i] == edges[i-1]:
        continue
    m += 1
print(maxV-minV+1, m)

u, v = edges[0]
print(u-minV, v-minV)
for i in range(1, len(edges)):
    if edges[i] == edges[i-1]:
        continue
    print(edges[i][0]-minV, edges[i][1]-minV)

# if minV == 0:
#     for u, v in edges:
#         print(u+1, v+1)
# else:
#     for u, v in edges:
#         print(u, v)
