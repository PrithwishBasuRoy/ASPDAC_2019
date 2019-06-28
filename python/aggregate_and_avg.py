# this code averages the value of a particular parameter for multiple pinned cells


import array
import numpy 
f=open("cell_fall_0","r")

list_cells_order=[]
list_cells={}
current_cell = ""
count = 1
each_count=1
for line in f:
	if count%8 == 1:
		if count == 1: 
			current_cell= line.replace("\n","")
			list_cells_order.append(current_cell)
			# each_count=1
			list_cells[current_cell]=each_count
		else:
			if current_cell != line.replace("\n","") :
				list_cells[current_cell]=each_count
				current_cell = line.replace("\n","")
				each_count=1
				# list_cells.append(current_cell)
				list_cells_order.append(current_cell)
				list_cells[current_cell]=each_count
			else :
				each_count=each_count+1
	count= count+1

f.close()

f=open("cell_fall_0","r")
f1=open("cell_fall_avg_0","w")
# print list_cells

# 3 + 3*7

for i in list_cells_order:
	# added_block=array.array()
	current_cell=i
	nos_of_count=list_cells[i]
	list_nos_1=numpy.zeros(7)
	list_nos_2=numpy.zeros(7)
	list_nos_3=numpy.zeros(7)
	list_nos_4=numpy.zeros(7)
	list_nos_5=numpy.zeros(7)
	list_nos_6=numpy.zeros(7)
	list_nos_7=numpy.zeros(7)


		
	for j in range(1,8*int(nos_of_count)+1):
		# list_nos_1=[0]*7
		# list_nos_2=[0]*7
		# list_nos_3=[0]*7
		# list_nos_4=[0]*7
		# list_nos_5=[0]*7
		# list_nos_6=[0]*7
		# list_nos_7=[0]*7
		# # list_nos_8=[]
		list_nos=[]
		if j % 8 ==1 :
			# current_cell=f.readline()
			f.readline()
			continue
		if j % 8 ==2 :
			a= f.readline().replace("\n","").split("\t")
			# print a
			for ele in a :
				list_nos.append(float(ele))
			# print list_nos
			
			count=0
			for ele1 in list_nos:	
				list_nos_1[count]=ele1+list_nos_1[count]
				count=count+1

			# print list_nos_1	
		if j % 8 ==3 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_2[count]=ele1+list_nos_2[count]
				count=count+1
			
		if j % 8 ==4 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_3[count]=ele1+list_nos_3[count]
				count=count+1	

		if j % 8 ==5 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_4[count]=ele1+list_nos_4[count]
				count=count+1

		if j % 8 ==6 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_5[count]=ele1+list_nos_5[count]
				count=count+1
					
		if j % 8 ==7 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_6[count]=ele1+list_nos_6[count]
				count=count+1
					
		if j % 8 ==0 :
			a= f.readline().replace("\n","").split("\t")
			for ele in a :
				list_nos.append(float(ele))
			count=0
			for ele1 in list_nos:	
				list_nos_7[count]=ele1+list_nos_7[count]
				count=count+1

	f1.write(current_cell+"\n")
	for ele in list_nos_1:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_2:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_3:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_4:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_5:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_6:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

	# f1.write(current_cell+"\n")
	for ele in list_nos_7:
		f1.write(str(round(ele/nos_of_count,6))+"\t")

	f1.write("\n")

f.close()
f1.close()		
