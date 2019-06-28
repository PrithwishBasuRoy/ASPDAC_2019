# f0=open("3v0.lib","r")

## This code fetches values of parameters for each of the particular gates
f0=open("3v0.lib","r")
f2=open("3v0.lib","r")


f_cell_leakage_power=open("cell_leakage_power_act_0","w")
# f_capacitance=open("capacitance_2","w")
# f_delay=open("delay_2","w")
# f_arrival=open("arrival_2","w")
# f_cumulative_slack=open("cumulative_slack_2","w")
# f_cell_rise=open("cell_rise_2","w")
# f_cell_fall=open("cell_fall_2","w")
# f_rise_transition= open("rise_transition_2" ,"w")
# f_fall_transition= open("fall_transition_2","w")

#size of lib 0
size_f0=0
for i in f0:
	size_f0=size_f0+1

#size of lib 2
size_f2=0
for i in f2:
	size_f2=size_f2+1


print(size_f0)
print(size_f2)

f0.close()
f2.close()
#f0=open("3v0.lib","r")
f0=open("3v0.lib","r")
f2=open("3v0.lib","r")
name_of_current_cell=""

reading_a_cell=False

#for i in range (0,1) :
#	print(i)
#	print(f2.readline())
#print(f0.readline())
count = 0
print(count)
print(size_f0)

# for the first library 
while count != size_f0:
	i = str(f0.readline())
	#print(i)
	k=0
	count=count+1
	if "cell(" in i :
		if "test_cell" not in i :
		
			
			reading_a_cell=True
			name_split_temp=i.split("(")	
			name_split=name_split_temp[1].split(")")
			name_of_current_cell=name_split[0]
			# print(name_of_current_cell)
			continue;

	if reading_a_cell == True and "cell_leakage_power :" in i :
			max_cap_temp= i.split(":")
			max_capacitance_of_current_cell= float(max_cap_temp[1].replace(";",""))
			# print(max_capacitance_of_current_cell)
			# f_capacitance.write(name_of_current_cell+"\n")
			# f_capacitance.write(str(max_capacitance_of_current_cell)+"\n")
			f_cell_leakage_power.write(name_of_current_cell+"\n")
			f_cell_leakage_power.write(str(max_capacitance_of_current_cell)+"\n")
			continue


	# if reading_a_cell == True and "cell_footprint :" in i :
	# 		cell_fp_temp= i.split(" : ")
	# 		cell_fp= cell_fp_temp[1].replace(";","")
	# 		# print(cell_fp)
	# 		continue
	
	# #print("------------------------------  Cell Power Values  --------------------------")

	# #if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "power(POWER_7x7)" in i :
	# if reading_a_cell == True  and "power(POWER_7x7)" in i :
	# 		print("------------------------------  Cell Power Values  --------------------------")			
	# 		#k=0
	# 		f_cell_leakage_power.write(name_of_current_cell + "\n")
	# 		cell_power_vals=[]
	# 		while(k<=9) :
	# 			if(k>1 and k<=9):
	# 				vals= f0.readline()					
	# 				print(vals)
	# 				cell_power_vals.append(vals)
	# 			else :
	# 				f0.readline()
	# 			k=k+1
	# 			if k == 9 :
	# 				k=0
	# 				break;
	# 		print("=------------------=-=-----------------------------=--==-"+str(len(cell_power_vals)))
	# 		for i in cell_power_vals:
	# 			#print i 
	# 			temp_vals=i.split("\"") 
	# 			temp_vals_c= temp_vals[1].split(",")
	# 			# print temp_vals_c
	# 			f_cell_leakage_power.write(temp_vals[1].replace(",","\t") + "\n")


	# #print("------------------------------  Cell Rise Values  --------------------------")

	# #if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "cell_rise(DELAY_7x7)" in i :
	# if reading_a_cell == True and "cell_rise(DELAY_7x7)" in i :
	# 		print("------------------------------  Cell Rise Values  --------------------------")			
	# 		#k=0
	# 		f_cell_rise.write(name_of_current_cell + "\n")
	# 		cell_rise_vals=[]
	# 		while(k<=9) :
	# 			if(k>1 and k<=9):
	# 				vals= f0.readline()					
	# 				#print(vals)
	# 				cell_rise_vals.append(vals)
	# 			else :
	# 				f0.readline()
	# 			k=k+1
	# 			if k == 9 :
	# 				k=0
	# 				break;
	# 		print("=------------------=-=-----------------------------=--==-"+str(len(cell_rise_vals)))
	# 		for i in cell_rise_vals:
	# 			#print i 
	# 			temp_vals=i.split("\"") 
	# 			temp_vals_c= temp_vals[1].split(",")
	# 			print temp_vals_c
	# 			f_cell_rise.write(temp_vals[1].replace(",","\t")+ "\n")
	# #print("------------------------------  Rise Transition Values  --------------------------")

	# #if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "rise_transition(DELAY_7x7)" in i :
	# if reading_a_cell == True and "rise_transition(DELAY_7x7)" in i :
	# 		#k=0
	# 		print("------------------------------  Rise Transition Values  --------------------------")
	# 		rise_trans_vals=[]
	# 		f_rise_transition.write(name_of_current_cell + "\n")			
	# 		while(k<=9) :
	# 			if(k>1 and k<=9):
	# 				vals= f0.readline()					
	# 				#print(vals)
	# 				rise_trans_vals.append(vals)
	# 			else :
	# 				f0.readline()
	# 			k=k+1
	# 			if k == 9 :
	# 				k=0
	# 				break;
	# 		print("=------------------=-=-----------------------------=--==-"+str(len(rise_trans_vals)))
	# 		for i in rise_trans_vals:
	# 			#print i 
	# 			temp_vals=i.split("\"") 
	# 			temp_vals_c= temp_vals[1].split(",")
	# 			print temp_vals_c
	# 			f_rise_transition.write(temp_vals[1].replace(",","\t")+ "\n")

	# #if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "fall_transition(DELAY_7x7)" in i :
	# if reading_a_cell == True and "fall_transition(DELAY_7x7)" in i :
	# 		#k=0
	# 		print("------------------------------  Fall Transition Values  --------------------------")
	# 		fall_trans_vals=[]
	# 		f_fall_transition.write(name_of_current_cell + "\n")
	# 		while(k<=9) :
	# 			if(k>1 and k<=9):
	# 				vals= f0.readline()					
	# 				#print(vals)
	# 				fall_trans_vals.append(vals)
	# 			else :
	# 				f0.readline()
	# 			k=k+1
	# 			if k == 9 :
	# 				k=0
	# 				break;
	# 		print("=------------------=-=-----------------------------=--==-"+str(len(fall_trans_vals)))
	# 		for i in fall_trans_vals:
	# 			#print i 
	# 			temp_vals=i.split("\"") 
	# 			temp_vals_c= temp_vals[1].split(",")
	# 			print temp_vals_c
	# 			f_fall_transition.write(temp_vals[1].replace(",","\t")+ "\n")
	# #print("------------------------------  Cell Fall Values  --------------------------")

	# #if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "cell_fall(DELAY_7x7)" in i :
	# if reading_a_cell == True  and "cell_fall(DELAY_7x7)" in i :
	# 		print("------------------------------  Cell Fall Values  --------------------------")			
	# 		#k=0
	# 		f_cell_fall.write(name_of_current_cell + "\n")
	# 		cell_fall_vals=[]
	# 		while(k<=9) :
	# 			if(k>1 and k<=9):
	# 				vals= f0.readline()					
	# 				#print(vals)
	# 				cell_fall_vals.append(vals)
	# 			else :
	# 				f0.readline()
	# 			k=k+1
	# 			if k == 9 :
	# 				k=0
	# 				break;
	# 		print("=------------------=-=-------------------------------==-"+str(len(cell_fall_vals)))
	# 		for i in cell_fall_vals:
	# 			#print i 
	# 			temp_vals=i.split("\"") 
	# 			temp_vals_c= temp_vals[1].split(",")
	# 			print temp_vals_c
	# 			f_cell_fall.write(temp_vals[1].replace(",","\t")+ "\n")	



# #for the second library 
# reading_a_cell=False
# while count != size_f2:
# 	i = str(f2.readline())
# 	#print(i)
# 	k=0
# 	count=count+1
# 	if "cell(" in i :
# 		if "test_cell" not in i :
		
# 			#print("YOLO "+str(count))	
# 			reading_a_cell=True
# 			name_split_temp=i.split("(")	
# 			name_split=name_split_temp[1].split(")")
# 			name_of_current_cell=name_split[0]
# 			print(name_of_current_cell)
# 			continue;

# 	if reading_a_cell == True and "max_capacitance :" in i :
# 			max_cap_temp= i.split(":")
# 			max_capacitance_of_current_cell= float(max_cap_temp[1].replace(";",""))
# 			print(max_capacitance_of_current_cell)
# 			f_capacitance.write(name_of_current_cell+"\n")
# 			f_capacitance.write(str(max_capacitance_of_current_cell)+"\n")
# 			continue


# 	if reading_a_cell == True and "cell_footprint :" in i :
# 			cell_fp_temp= i.split(" : ")
# 			cell_fp= cell_fp_temp[1].replace(";","")
# 			print(cell_fp)
# 			continue
	
# 	#print("------------------------------  Cell Power Values  --------------------------")

# 	#if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "power(POWER_7x7)" in i :
# 	if reading_a_cell == True  and "power(POWER_7x7)" in i :
# 			print("------------------------------  Cell Power Values  --------------------------")			
# 			#k=0
# 			f_cell_leakage_power.write(name_of_current_cell + "\n")
# 			cell_power_vals=[]
# 			while(k<=9) :
# 				if(k>1 and k<=9):
# 					vals= f0.readline()					
# 					#print(vals)
# 					cell_power_vals.append(vals)
# 				else :
# 					f0.readline()
# 				k=k+1
# 				if k == 9 :
# 					k=0
# 					break;
# 			print("=------------------=-=-----------------------------=--==-"+str(len(cell_power_vals)))
# 			for i in cell_power_vals:
# 				#print i 
# 				temp_vals=i.split("\"") 
# 				temp_vals_c= temp_vals[1].split(",")
# 				print temp_vals_c
# 				f_cell_leakage_power.write(temp_vals[1].replace(",","\t") + "\n")


# 	#print("------------------------------  Cell Rise Values  --------------------------")

# 	#if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "cell_rise(DELAY_7x7)" in i :
# 	if reading_a_cell == True and "cell_rise(DELAY_7x7)" in i :
# 			print("------------------------------  Cell Rise Values  --------------------------")			
# 			#k=0
# 			f_cell_rise.write(name_of_current_cell + "\n")
# 			cell_rise_vals=[]
# 			while(k<=9) :
# 				if(k>1 and k<=9):
# 					vals= f0.readline()					
# 					#print(vals)
# 					cell_rise_vals.append(vals)
# 				else :
# 					f0.readline()
# 				k=k+1
# 				if k == 9 :
# 					k=0
# 					break;
# 			print("=------------------=-=-----------------------------=--==-"+str(len(cell_rise_vals)))
# 			for i in cell_rise_vals:
# 				#print i 
# 				temp_vals=i.split("\"") 
# 				temp_vals_c= temp_vals[1].split(",")
# 				print temp_vals_c
# 				f_cell_rise.write(temp_vals[1].replace(",","\t")+ "\n")
# 	#print("------------------------------  Rise Transition Values  --------------------------")

# 	#if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "rise_transition(DELAY_7x7)" in i :
# 	if reading_a_cell == True and "rise_transition(DELAY_7x7)" in i :
# 			#k=0
# 			print("------------------------------  Rise Transition Values  --------------------------")
# 			rise_trans_vals=[]
# 			f_rise_transition.write(name_of_current_cell + "\n")			
# 			while(k<=9) :
# 				if(k>1 and k<=9):
# 					vals= f0.readline()					
# 					#print(vals)
# 					rise_trans_vals.append(vals)
# 				else :
# 					f0.readline()
# 				k=k+1
# 				if k == 9 :
# 					k=0
# 					break;
# 			print("=------------------=-=-----------------------------=--==-"+str(len(rise_trans_vals)))
# 			for i in rise_trans_vals:
# 				#print i 
# 				temp_vals=i.split("\"") 
# 				temp_vals_c= temp_vals[1].split(",")
# 				print temp_vals_c
# 				f_rise_transition.write(temp_vals[1].replace(",","\t")+ "\n")

# 	#if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "fall_transition(DELAY_7x7)" in i :
# 	if reading_a_cell == True and "fall_transition(DELAY_7x7)" in i :
# 			#k=0
# 			print("------------------------------  Fall Transition Values  --------------------------")
# 			fall_trans_vals=[]
# 			f_fall_transition.write(name_of_current_cell + "\n")
# 			while(k<=9) :
# 				if(k>1 and k<=9):
# 					vals= f0.readline()					
# 					#print(vals)
# 					fall_trans_vals.append(vals)
# 				else :
# 					f0.readline()
# 				k=k+1
# 				if k == 9 :
# 					k=0
# 					break;
# 			print("=------------------=-=-----------------------------=--==-"+str(len(fall_trans_vals)))
# 			for i in fall_trans_vals:
# 				#print i 
# 				temp_vals=i.split("\"") 
# 				temp_vals_c= temp_vals[1].split(",")
# 				print temp_vals_c
# 				f_fall_transition.write(temp_vals[1].replace(",","\t")+ "\n")
# 	#print("------------------------------  Cell Fall Values  --------------------------")

# 	#if reading_a_cell == True and name_of_current_cell == "AN2B1CHD" and "cell_fall(DELAY_7x7)" in i :
# 	if reading_a_cell == True  and "cell_fall(DELAY_7x7)" in i :
# 			print("------------------------------  Cell Fall Values  --------------------------")			
# 			#k=0
# 			f_cell_fall.write(name_of_current_cell + "\n")
# 			cell_fall_vals=[]
# 			while(k<=9) :
# 				if(k>1 and k<=9):
# 					vals= f0.readline()					
# 					#print(vals)
# 					cell_fall_vals.append(vals)
# 				else :
# 					f0.readline()
# 				k=k+1
# 				if k == 9 :
# 					k=0
# 					break;
# 			print("=------------------=-=-----------------------------=--==-"+str(len(cell_fall_vals)))
# 			for i in cell_fall_vals:
# 				#print i 
# 				temp_vals=i.split("\"") 
# 				temp_vals_c= temp_vals[1].split(",")
# 				print temp_vals_c
# 				f_cell_fall.write(temp_vals[1].replace(",","\t")+ "\n")	








f_cell_leakage_power.close()
# f_capacitance.close()
# f_delay.close()
# f_arrival.close()
# f_cumulative_slack.close()
# f_cell_rise.close()
# f_cell_fall.close()
# f_rise_transition.close()
# f_fall_transition.close()
f0.close()
f2.close()









print(count)
