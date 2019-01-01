"""Generates random .map files"""
import random as random
import os

LWER_BND = 0
UPER_BND = 3
X_BND = 10
Y_BND = 8

def main():
	path = "assets/maps/"
	for i in range(6):
		filename = "map"+str(i)+".map"
		fh = open(os.path.join(path, filename), "w")

		for y in range(Y_BND):
			for x in range(X_BND):
				#generate random numbers between the bounds
				r = random.randint(LWER_BND, UPER_BND)
				# weight the probability in favor of clear blocks
				w = random.randint(0, 100)
				if w < 60:
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

				r = "0"+str(r)+" "
				fh.write(r)
			fh.write('\n')

		print("generated map"+str(i))
		fh.close()

if __name__ == '__main__':
	main()
