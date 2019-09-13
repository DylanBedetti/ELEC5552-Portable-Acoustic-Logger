with open("ACCDATA3.bin", "rb") as file:
	while file!= "":

		#16 bit ints are 2 bytes each 
		angle_x=int.from_bytes(file.read(2), byteorder='big')
		angle_y=int.from_bytes(file.read(2), byteorder='big')
		angle_z=int.from_bytes(file.read(2), byteorder='big')
		accX=int.from_bytes(file.read(2), byteorder='big')
		accY=int.from_bytes(file.read(2), byteorder='big')
		accZ=int.from_bytes(file.read(2), byteorder='big')	

		#Convert to 16 bit signed int
		if(angle_x>32767):
			angle_x=angle_x-65536
		if(angle_y>32767):
			angle_y=angle_y-65536
		if(angle_z>32767):
			angle_z=angle_z-65536

		if(accX>32767):
			accX=accX-65536
		if(accY>32767):
			accY=accY-65536
		if(accZ>32767):
			accZ=accZ-65536
			
		f = open('accTXT.txt', 'a')
		#Read in values and then conver to +-%/s and +-g
		f.write(str(angle_x/65.5)+","+str(angle_y/65.5)+","+str(angle_z/65.5)+","+str(accX/16384)+","+str(accY/16384)+","+str(accZ/16384)+"\n")
		f.close()

file.close()
