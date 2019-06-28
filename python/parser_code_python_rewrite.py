import os
import sys
filename=sys.stdin.readline().split()[0]
print filename
filelist = [ ]
filelist.append(filename)



'''
This is an example netlist:

module bw_clk_cl_sparc_cmp ( so, adbginit_l ); 
                This section is called the port list
input si, se, adbginit_l;
            This section is called the input section
output so, rclk, dbginit_l, cluster_grst_l;
            This section is called the output section
wire   rclk, n2, \I0/n1 , \I0/net35729 ;
            This section is called the wire section


assign rclk = gclk;
BUFLTX1 U1 ( .I(n2), .O(cluster_grst_l) );
            The two lines above are called the cells section
endmodule
 '''
for file in filelist:
        
        outfile=open(file+'_parsed.v','w+')
        input_flag=False #set to true when inputs are being parsed
        output=False # set to true when outputs are being parsed
        wire=False #set to true when wires are being parsed
        cells=False #set to true when cells are being parsed
        write=False # set to true when cells are being written to file
        tempstring=' '
        strlist=[]
        count=1000# please check if the max instance count (eg: BUFLTX1 U100 , 100 is the instance count), is lesser than count.
        with open(file) as infile:
            for line in infile:
                if 'input' in line and not input_flag:
                    '''At this point the parser has finished reading the port list of the verilog file. We write it in the prescribed format to the outfile and append //Start PIs and begin parsing the input section of the verilog file'''
        #            print tempstring
                    a=tempstring.split(',')
                    print 'a'
                    print a 
        #            print 'temp' 
                    temp=a[0].split('(')
		    print temp
                    print 'temp'
                    print temp[1]
                    outfile.write(temp[0]+'(\n')
                    outfile.write(temp[1]+',\n')
                    for i in range(1,len(a)-1):
                        outfile.write(a[i]+',\n')
                    outfile.write(a[len(a)-1]+')\n')
                    tempstring=' '
                    outfile.write('//Start PIs\n')
        
                    input_flag=True
                elif 'output' in line:
                    '''At this point the parser has finished reading the port list of the verilog file. We write the PIs in the presecribed format to the outfile and append //Start POs and begin parsing the output section of the verilog file '''
        #            print tempstring
                    a=tempstring.split(',')
                    outfile.write(a[0]+';\n')
                    for i in range(1,len(a)-1):
                        outfile.write('input '+a[i]+';\n')
                    outfile.write('input '+a[len(a)-1]+'\n')
                    outfile.write('//Start POs\n')
                    tempstring=' '
                    input_flag=False
                    output=True
                elif 'wire' in line:
                    '''At this point the parser has finished reading the PIs & POs . We write the POs in ISPD format and start parsing wires'''
                    a=tempstring.split(',')
                    outfile.write(a[0]+';\n')
                    for i in range(1,len(a)-1):
                        outfile.write('output '+a[i]+';\n')
                    outfile.write('output '+a[len(a)-1]+'\n')
        
                    outfile.write('//Start wires\n')
                    tempstring=' '
                    output=False
                    wire=True
                elif 'endmodule' in line:
        
                    write=True
                    cells=False
                if not input_flag and not output and not wire and not cells and 'module' in line:
                    #print line.strip()
                    tempstring=tempstring+line.strip()
                elif input_flag:
                    tempstring=tempstring+line.strip()
                elif output:
                    tempstring=tempstring+line.strip()
                elif wire:
                    tempstring=tempstring+line.strip()
                    if ';' in line:
                         #tempstring=tempstring+line.strip()
                         a=tempstring.split(',')
                         #print a
                         outfile.write(a[0]+';\n')
                         if(len(a)>1):
                             for i in range(1,len(a)-1):
                                 outfile.write('wire '+a[i]+';\n')
                             outfile.write('wire '+a[len(a)-1]+'\n')
                         tempstring=' '
                         outfile.write('//Start cells\n')
                         wire=False
                         cells=True
        
                elif cells:
                    '''This section parses the gates, It converts the assign statements into buffers, please check that the count variable is set to a value greater than the max instance count in your netlist ***VVIP****'''
                    if 'assign' in line:
                        if('b0' not in line and 'b1' not in line):
                            st=line.strip().split(';')[0].split()
                            #print st
                            templine='BUFX1 U'+str(count)+' ( .I1('+st[3]+'), .O('+st[1]+') );\n'
                            tempstring=tempstring+templine.strip()
                            count=count+1
                    else:
                        tempstring=tempstring+line.strip()
                elif write:
                     strlist=tempstring.split(';')
        
                     for i in range(0,len(strlist)):
                         if(len(strlist)>0):
                            outfile.write(strlist[i]+';\n')
                     outfile.write('endmodule')
                     write=False

        



