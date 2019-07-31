#This code removes the cells that are not present in the lvt hvt mapping 
f_target_lib=open("3v2.lib","r")
# f_target_lib=open(".lib","r")
f_output_lib=open("Final_HVT.lib","w")
f_look_up_file=open("LVT_HVT_mapping_341_29_j_fin","r")


list_lvt =[]
list_hvt =[]
gate_token_lvt_mapping={}
gate_token_hvt_mapping={}
token_hvt=[]

# commented for the time being
count =0
#creates a dictionary for the mapping - will be later used for cell removal
for i in f_look_up_file :
	cells = i.split(":")
	list_lvt.append(cells[0])
	list_hvt.append(cells[1].replace("\n",""))
	gate_token_lvt_mapping[cells[0]]="token_"+str(count)+"_lvt"
	gate_token_hvt_mapping[cells[1].replace("\n","")]="token_"+str(count)+"_hvt" 		
	# print(cells[1].replace("\n",""))
	# token_lvt.append("token_"+str(count)+"_lvt")
	# token_lvt.append("token_"+str(count)+"_hvt")
	# f_lvt_gate_lookup.write(cells[0]+":"+"token_"+str(count)+"_lvt\n")
	# f_hvt_gate_lookup.write(cells[1].replace("\n","")+":"+"token_"+str(count)+"_hvt\n")
	count=count+1

# print list_hvt	
# print list_lvt	
list_deleted=[]
list_retained=[]
list_starting_position_allowed_cell=[]
list_starting_position_all_cell=[]
list_ending_position_allowed_cell=[]
list_starting_position_all_cell=[]

present_cell=""
ignore_lines=False
count = 1
nos_of_cells= 0
list_starting_position_allowed_cell.append(1)
list_starting_position_all_cell.append(1)
for i in f_target_lib:
	if "cell(" in i  :
		# print("I am here!!"+ i)
		if("test_cell" not in i):	
			reading_a_cell=True
			name_split_temp=i.split("(")	
			name_split=name_split_temp[1].split(")")
			name_of_current_cell=name_split[0]
			print name_of_current_cell	
			list_starting_position_all_cell.append(count)
			if(name_of_current_cell in list_hvt) :
				list_starting_position_allowed_cell.append(count)
				
			
	count = count +1			

print list_starting_position_allowed_cell
print("------------------------------------")
print list_starting_position_all_cell
print count
list_starting_position_all_cell.append(count+1)

f_target_lib.close()

f_target_lib=open("3v2.lib","r")
line_no = 1
while(line_no<count):
	if(line_no in list_starting_position_allowed_cell):
		pos=list_starting_position_all_cell.index(line_no)
		end_pos=list_starting_position_all_cell[pos+1]
		while(line_no!=end_pos):
			f_output_lib.write(f_target_lib.readline())
			line_no=line_no+1

	elif(line_no == count):
		f_output_lib.write(f_target_lib.readline())
	else:
		f_target_lib.readline()
		line_no=line_no+1		
f_output_lib.write("}\n")







#don't touch this 
# list_lvt=['AN2B1BUFX1']

# present_cell=""
# ignore_lines=False
# for i in f_target_lib:
# 	if "cell(" in i  :
# 		print("I am here!!"+ i)
# 		if("test_cell" not in i):	
# 			reading_a_cell=True
# 			name_split_temp=i.split("(")	
# 			name_split=name_split_temp[1].split(")")
# 			name_of_current_cell=name_split[0]
# 			print name_of_current_cell	

# 			# if (name_of_current_cell != present_cell):
# 			if(name_of_current_cell not in list_lvt ) :
# 				ignore_lines=True
# 			else :
# 				f_output_lib.write(i)
# 				ignore_lines =False
# 				# present_cell=name_of_current_cell	
# 			continue
# 		else :
# 			if(ignore_lines == False) :
# 				f_output_lib.write(i)
# 			continue		

# 	else :
# 		print i
# 		if "/**/" in i:
# 			ignore_lines = False
# 			continue

# 		else:
# 			if(ignore_lines==False) :
# 				f_output_lib.write(i)
# 				continue
			


		# print(name_of_current_cell)
		# continue;

for i in list_hvt:

	count_i=0
	for j in list_hvt:
		if i == j:
			count_i=count_i+1
	print(i +" "+ str(count_i))	


