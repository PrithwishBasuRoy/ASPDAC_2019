#This program returns the cell names of the nodes given as input (excluding the PI and the PO nodes)

# list_of_nodes=[20004,8515,35730,41076,101884,20152,20011,51113,26091,26090,51109,102482]
#list_of_nodes=[8470,34941,90979,114959,97723,28195,89227,12963,12964,97720]

list_of_nodes=[]
# f=open("replaced_gates_full_circuit_big.txt","r")
f=open("replaced_gates_slew_circuit_mod_30_08_03.txt","r")
fn=open("list_of_replaced_gates_slew_circuit_mod_30_08_03.txt","w")
all_gates_temp=[]
for line in f:
	all_gates_temp  = line.split(",")
for gates in all_gates_temp:
	list_of_nodes.append(int(gates.replace(" ","")))

# list_of_nodes=[52995,45182,59989,97659,27445,27444,110125,66346,66354,40912,66357]
list_of_gate_names=[]
list_of_gate_names_str=[]

fixed_node= 8263
fixed_nodes_cell_name=112482 


for i in list_of_nodes:

	cellname_for_current_node= fixed_nodes_cell_name+i-8263
	if (cellname_for_current_node >= 237578) :
		cellname_for_current_node = cellname_for_current_node + 1
	list_of_gate_names.append(cellname_for_current_node)
	list_of_gate_names_str.append("U"+ str(cellname_for_current_node))
	fn.write("U"+ str(cellname_for_current_node))

print list_of_gate_names_str	


map_dict={}
mapper=open("LVT_MAP","r")
for i in mapper:
	print i
	lvt_hvt=i.split(":")
	map_dict[lvt_hvt[0].replace(" ","")]=lvt_hvt[1].replace(" ","").replace("\n","")

print map_dict

verilog_r= open("aes_128_synthesized_single_flop_HHD.v","r")
verilog_w= open("aes_128_circuit_30_08_03.v","w")

# i=29900
for line in verilog_r:
	line_split = line.split(" ")
	# print line_split[0].replace(" ","")
	gate_type = ""
	gate_name = ""
	if len(line_split) >2 :
		gate_type = line_split[2].replace(" ","")
	if len(line_split) >3 :
		gate_name = line_split[3].replace(" ","")

	if gate_name in list_of_gate_names_str:
		mod_line = line.replace(gate_type,map_dict[gate_type])		
		verilog_w.write(mod_line)
	else :
		verilog_w.write(line)	
