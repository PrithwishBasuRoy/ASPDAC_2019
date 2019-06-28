# This code assigns cellwise parity values , the strategy used to compute the parity is to sum up the nos of unate
f0= open("3v2.lib","r")
size_f0=0
for i in f0:
	size_f0=size_f0+1

f1=open("parity_2","w")//parity hvt files 
f0.close()

f0= open("3v2.lib","r")

name_of_current_cell=""

reading_a_cell=False

count = 0
k=0
unate_list=[]

while count != size_f0:
	i = str(f0.readline())
	#print(i)
	# k=0
	# count=count+1
	if "cell(" in i :
		if "test_cell" not in i :
			print k
			if(k!=0):
				f1.write(name_of_current_cell+"\t")
				sum=0
				# count_p=0
				# count_n=0
				# count_z=0
				for l in unate_list:
					sum=sum+l
					# if l == -1:
					# 	count_n=count_n+1
					# if l == 1:	
					# 	count_p=count_p+1
					# if l == 0:	
					# 	count_z=count_z+1	
				if sum > 0 :
					# f1.write(name_of_current_cell+"\n")
					f1.write(str(1)+"\n")
				elif sum < 0 :	
					# f1.write(name_of_current_cell+"\n")
					f1.write(str(-1)+"\n")
				else :
					# f1.write(name_of_current_cell+"\n")
					f1.write(str(0)+"\n")
				# f1.write(str(count_p)+"||"+str(count_n)+"||"+str(count_z)+"\n")	
				
			#print("YOLO "+str(count))	
			reading_a_cell=True
			name_split_temp=i.split("(")	
			name_split=name_split_temp[1].split(")")
			name_of_current_cell=name_split[0]
			# print(name_of_current_cell)
			unate_list=[]
			k=k+1	

			continue;


	if reading_a_cell == True and "timing_sense :" in i :
			timing_sense_temp= i.split(":")
			timing_of_current_cells_current_pin= timing_sense_temp[1].replace(";","")
			# print(max_capacitance_of_current_cell)
			# f_capacitance.write(name_of_current_cell+"\n")
			# f_capacitance.write(str(max_capacitance_of_current_cell)+"\n")
			if "positive_unate" in timing_of_current_cells_current_pin:
				unate_list.append(1)
			elif "negative_unate" in timing_of_current_cells_current_pin:	
				unate_list.append(0)
			else :
				unate_list.append(0)
			continue

	count=count+1




f0.close()
f1.close()
