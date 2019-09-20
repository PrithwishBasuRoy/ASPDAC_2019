#This program returns the cell names of the nodes given as input (excluding the PI and the PO nodes)

list_of_nodes=[]
list_of_gate_names=[]
# list_of_gate_names_str=['U124223', 'U112734', 'U139949', 'U145295', 'U206103', 'U124371', 'U124230', 'U155332', 'U130310', 'U130309', 'U155328', 'U206701']
#list_of_gate_names_str=['U233407/O','U191235/O', 'U183324/O', 'U228311/O', 'U250767/O', 'U251115/O', 'U222227/O', 'U196415/O', 'U196414/O']
# list_of_gate_names_str=['U210382/O','U210382/O', 'U210382/O', 'U247161/O', 'U249484/O', 'U217668/O', 'U225039/O', 'U131588/O', 'U131587/O']
# list_of_gate_names_str=['U188289/O','U182838/O','U236950/O','U219822/O','U130513/O','U219770/O','U193375/O','U141609/O','U192439/O']
# list_of_gate_names_str=['U221444/O','U228698/O','U117887/O','U117886/O','U132932/O','U132931/O','U208210/O','U154739/O','U124510/O','U124509/O']
# list_of_gate_names_str=['U237578/O','U242504/O','U242505/O']
# list_of_gate_names_str=['U205585/O','U238095/O']
# list_of_gate_names_str=['U205585/O','U238095/O']
#list_of_gate_names_str=['U121938/O','U202284/O','U160484/O','U142137/O','U142136/O','U228293/O']
#list_of_gate_names_str=['U130516/O','U124380/O','U113313/O','U166973/O','U195156/O','U219875/O','U172824/O','U173260/O','U148667/O','U148666/O'] 
#list_of_gate_names_str=['U180775/O','U113678/O','U234264/O','U227934/O','U133029/O','U229502/O','U178716/O','U143411/O','U164860/O','U177610/O','U143345/O','U191552/O','U113957/O','U201266/O']
#list_of_gate_names_str=['U186376/O','U193467/O','U234984/O','U240651/O','U160279/O','U214895/O','U126859/O','U126858/O',' U118793/O','U204467/O']
#list_of_gate_names_str=['U206962/O','U173143/O','U148429/O','U148431/O','U161806/O','U111165/O','U140196/O','U158498/O','U124000/O','U117158/O']
#list_of_gate_names_str=['U147943/O','U208725/O','U154529/O','U170144/O',' U170143/O',' U154567/O','U168580/O','U176178/O','U185852/O'] 
# list_of_gate_names_str=['U182185/O','U166146/O','U134733/O','U156727/O','U138441/O','U151812/O','U171636/O','U171635/O','U171635/O','U164905/O','U164904/O','U145500/O']
#list_of_gate_names_str=['U14847/O','U14846/O','U9488/O','U19108/O','U19109/O','U14869/O','U9498/O','U14876/O','U19121/O','U19122/O','U9902/O','U16091/O','U9889/O','U19124/O','U19125/O','U19135/O']
#list_of_gate_names_str=['U915/O','U962/O','U777/O','U1659/O','U1661/O']

#list_of_gate_names_str=['U14489/O','U14490/O','U10973/O','U10739/O','U8218/O','U11518/O','U11516/O','U7523/O','U7697/O','U10748/O','U8047/O','U7660/O','U7659/O','U14358/O','U13923/O','U14909/O','U8949/O','U13434/O','U13172/O','U7460/O','U13801/O','U15479/O']
list_of_gate_names_str=['U14624/O','U14626/O','U9514/O','U15421/O','U9503/O','U9490/O','U15424/O','U11812/O','U15427/O','U15431/O','U12575/O','U9427/O']
#list_of_gate_names_str=[]
# f=open("list_of_gate_in_r8_r9","r")
# f=open("test_gates_r8_r9","r")
# fn=open("list_of_nodes_test_gates_r8_r9","w")
# for line in f:
# 	all_gates_temp  = line.split(",")
# for gates in all_gates_temp:
# 	list_of_gate_names_str.append(gates.replace(" ",""))		

print list_of_gate_names_str

# list_of_gate_names_str=['U216187/O','U235769/O','U235768/O','U140259/O','U155212/O','U145591/O','U117126/O','U131374/O']
fixed_node= 930
fixed_nodes_cell_name=6504 


for i in list_of_gate_names_str:
	cell_name_temp=i.replace("U","").replace("/O","") 
	print cell_name_temp

	if cell_name_temp!= "":
		node_number_for_cell_name= fixed_node+int(cell_name_temp)-fixed_nodes_cell_name
		# if(node_number_for_cell_name >= 133359):
		# 	node_number_for_cell_name=node_number_for_cell_name-1
		list_of_nodes.append(node_number_for_cell_name)
		print("--------------------------------------")
		print node_number_for_cell_name
		# fn.write(str(node_number_for_cell_name)+"\n");

	# list_of_gate_names_str.append("U"+ str(node_number_for_cell_name))

print list_of_nodes
# print list_of_gate_names_str
# print("++++++++++++++++++++++++++++++++++++++++================+++++++++++++++++++++++++++++++")
# print list_of_nodes

# print("if("),
# for i in list_of_nodes:
# 	if i!= list_of_nodes[len(list_of_nodes) - 1] :
# 		print("j=="+str(i)+"||"),
# 	else:
# 		print("j=="+str(i)+")")
