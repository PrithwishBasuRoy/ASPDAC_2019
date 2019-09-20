#This program returns the cell names of the nodes given as input (excluding the PI and the PO nodes)

list_of_nodes=[]
list_of_gate_names=[]
# list_of_gate_names_str=['U124223', 'U112734', 'U139949', 'U145295', 'U206103', 'U124371', 'U124230', 'U155332', 'U130310', 'U130309', 'U155328', 'U206701']
#list_of_gate_names_str=['U233407/O','U191235/O', 'U183324/O', 'U228311/O', 'U250767/O', 'U251115/O', 'U222227/O', 'U196415/O', 'U196414/O']
# list_of_gate_names_str=['U210382/O','U210382/O', 'U210382/O', 'U247161/O', 'U249484/O', 'U217668/O', 'U225039/O', 'U131588/O', 'U131587/O']
# list_of_gate_names_str=['U188289/O','U182838/O','U236950/O','U219822/O','U130513/O','U219770/O','U193375/O','U141609/O','U192439/O']
# list_of_gate_names_str=['U221444/O','U228698/O','U117887/O','U117886/O','U132932/O','U132931/O','U208210/O','U154739/O','U124510/O','U124509/O']
#list_of_gate_names_str=['U242504/O','U242505/O']
list_of_gate_names_str=[]
f=open("list_of_gate_in_r8_r9","r")
#f=open("test_gates_r8_r9","r")
fn=open("list_of_nodes_in_r8_r9_corrected","w")
for line in f:
	all_gates_temp  = line.split(",")
for gates in all_gates_temp:
	list_of_gate_names_str.append(gates.replace(" ",""))		

print list_of_gate_names_str

# list_of_gate_names_str=['U216187/O','U235769/O','U235768/O','U140259/O','U155212/O','U145591/O','U117126/O','U131374/O']
fixed_node= 8263
fixed_nodes_cell_name=112482 


for i in list_of_gate_names_str:
	cell_name_temp=i.replace("U","").replace("/O","") 
	print cell_name_temp

	if cell_name_temp!= "":
		node_number_for_cell_name= fixed_node+int(cell_name_temp)-fixed_nodes_cell_name
		if(node_number_for_cell_name >= 133359):
			node_number_for_cell_name=node_number_for_cell_name-1
		list_of_nodes.append(node_number_for_cell_name)
		fn.write(str(node_number_for_cell_name)+"\n");
	# list_of_gate_names_str.append("U"+ str(node_number_for_cell_name))

# print list_of_gate_names_str
# print("++++++++++++++++++++++++++++++++++++++++================+++++++++++++++++++++++++++++++")
# print list_of_nodes

# print("if("),
# for i in list_of_nodes:
# 	if i!= list_of_nodes[len(list_of_nodes) - 1] :
# 		print("j=="+str(i)+"||"),
# 	else:
# 		print("j=="+str(i)+")")
