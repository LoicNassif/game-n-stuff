"""Generates random .map files"""
import random as random
import os

LWER_BND = 0
UPER_BND = 3
X_BND = 15
Y_BND = 12

def main():
	path = "assets/maps/"
	for i in range(1,9):
		end_tile = False
		invalid_tile = True
		while not end_tile or invalid_tile:
			invalid_tile = False
			end_tile = False
			filename = "map"+str(i)+".map"
			fh = open(os.path.join(path, filename), "w")
			for y in range(Y_BND):
				for x in range(X_BND):
					#generate random numbers between the bounds
					r = random.randint(LWER_BND, UPER_BND)
					# weight the probability in favor of clear blocks
					w = random.randint(0, 100)
					if w < 35:
						r = 3

					# Positions around spawn needs to be clear
					if (x < 2) and (y < 3): # P1
						r = 3
					elif (y == 0) and (x == X_BND - 2):
						r = 3
					elif (y == 0) and (x == X_BND - 1):
						r = 3
					elif (y == 1) and (x == X_BND - 2):
						r = 3
					elif (y == 1) and (x == X_BND - 1):
						r = 3
					elif (y == 2) and (x == X_BND - 2):
						r = 3
					elif (y == 2) and (x == X_BND - 1):
						r = 3

					if (r == 1) and (not end_tile) and (int(X_BND/2 - 3) < x < int(X_BND/2 + 3)) and (int(Y_BND/2 - 3) < y < int(Y_BND/2 + 3)):
						end_tile = True
						r = "0"+str(r)+" "
						fh.write(r)
					elif (r != 1):
						r = "0"+str(r)+" "
						fh.write(r)
					elif (r == 1) and end_tile:
						if random.randint(0,1) == 0:
							r += 1
						else:
							r -= 1
						r = "0"+str(r)+" "
						fh.write(r)
					elif ((int(X_BND/2 - 3) < x < int(X_BND/2 + 3)) and (int(Y_BND/2 - 3) < y < int(Y_BND/2 + 3))):
						r = 1
						end_tile = True
						r = "0"+str(r)+" "
						fh.write(r)
					elif (r == 1):
						if random.randint(0,1) == 0:
							r += 1
						else:
							r -= 1
						r = "0"+str(r)+" "
						fh.write(r)						
					else:
						invalid_tile = True

				fh.write('\n')
			fh.close()
		print("generated map"+str(i))
if __name__ == '__main__':
	main()
