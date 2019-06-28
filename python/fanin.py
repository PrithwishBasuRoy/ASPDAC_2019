import os
import sys
filename=sys.stdin.readline().split()[0]
print filename

from collections import defaultdict
vertex_array=defaultdict(list)
with open('../benchmarks/'+filename+'.hgr') as infile:
    for i,line in enumerate(infile):
        if(i==0):
            a=int(line.split()[0])
            for j in range(0,a):
                vertex_array[j]=[]
        if(i>0):
            a=line.split()
            for j in range(0,len(a)):
#                print a[j]
                if int(a[j]) not in vertex_array:
                    vertex_array[int(a[j])].append(i-1)
                else:
                    vertex_array[int(a[j])].append(i-1)
            #print vertex_array[int(a[j])]
        
outfile=open('../benchmarks/'+filename+'.fanin','w')
for i in range(0,len(vertex_array)):
    for j in range(0,len(vertex_array[i])):
        outfile.write(str(vertex_array[i][j])+'\t')
    outfile.write('\n')
