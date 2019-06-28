import os
import sys
picount=0
pocount=0
cellcount=0
flopcount=0
floplist=[]
totalgates=0
filename=sys.stdin.readline().split()[0]
print filename

with open( '../benchmarks/'+filename+'.type') as infile:
    for i,line in enumerate(infile):
        totalgates+=1
        if('ms' in line or 'QDF' in line or 'DFF' in line):
            print i
            floplist.append(i)
            flopcount+=1
        elif('PI' in line):
            picount+=1
        elif('PO' in line):
            pocount+=1
        else:
            cellcount+=1
infile.close()
print 'total cells: '+str(totalgates)+'\n'
print 'flops: '+str(flopcount)+'\n'
print 'pi_s: '+str(picount)+'\n'
print 'po_s: '+str(pocount)+'\n'
print 'cellcount: '+str(cellcount)+'\n'
print floplist
outfile=open('../benchmarks/'+filename+'.adj','w+')
outfile.write(str(totalgates+flopcount)+'\n')
typefile=open('../benchmarks/'+filename+'.typ','w+')
#flop_count=1
a=' '
#write=False
with open('../benchmarks/'+filename+'.hgr') as infile:
     for i,line in enumerate(infile):

        write=True
        if(i>=1):
            print i
            a=line.split()
            print a
            for j in range(0,len(a)):
                if int(a[j]) in floplist:
                    a[j]=totalgates+floplist.index(int(a[j]))
                    outfile.write(str(a[j])+'\t')
                    
                
                  #  print str(a[j])+' present in line: '+line+' and is replaced with '+str(totalgates+floplist.index(a[j])+1)+'\n'

 #           if(write==True):
                else:
                    outfile.write(a[j]+'\t')
            outfile.write('\n')
    
infile.close()
for i in range(0,flopcount):
    outfile.write('\n')
with open('../benchmarks/'+filename+'.type') as infile:
    for i,line in enumerate(infile):
        if('ms' in line or 'QDF' in line or 'DFF' in line):
            typefile.write('PSEUDOPI\n')
        else:
            typefile.write(line)

    for k in range(0,flopcount):
        typefile.write('PSEUDPO\n')
