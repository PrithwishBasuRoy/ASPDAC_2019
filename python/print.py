count=0
outfile=open('contest_rvt.cellfalltransition','w+')
outfile1=open('contest_hvt.cellfalltransition','w+')
outfile2=open('contest_lvt.cellfalltransition','w+')
with open('contest.cellfalltransition') as infile:
    for line in infile:
        if 'm' in line:
            rvt=True
            hvt=False
            lvt=False
        elif 's' in line:
            hvt=True
            rvt=False
            lvt=False
        elif 'f' in line:
            hvt=False
            rvt=False
            lvt=True

        if rvt and count<=8:
            count+=1
            outfile.write(line)
            if(count==8):
                count=0
                rvt=False

        elif hvt and count<=8:
            count+=1
            outfile1.write(line)
            if(count==8):
                count=0
                hvt=False
        elif lvt and count<=8:
            count+=1
            outfile2.write(line)
            if(count==8):
                count=0
                lvt=False
        
print count


#outfile=open('contest_rvt.capacitance','w+')
#outfile1=open('contest_hvt.capacitance','w+')
#outfile2=open('contest_lvt.capacitance','w+')
#with open('contest.capacitance') as infile:
#    for line in infile:
#        if 'm' in line:
#            outfile.write(line)
#        elif 's' in line:
#            outfile1.write(line)
#        elif 'f' in line:
#            outfile2.write(line)
