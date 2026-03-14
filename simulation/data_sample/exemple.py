#code to generate random data for the 3 sensors of the project and test if the code works
import random
import math

dataFile = open("simulation/data_sample/sample.csv", "w")
dataFile.write("s1,s2,s3,x,y\n")

n = int(input("Enter the number of elements for the: "))
sensor = [[0, 0], [0, 0], [0, 0]]

R = float(input("Radius = "))
sensor = [[0, R], [R * math.sqrt(3) / 2.0, -R / 2.0], [-R * math.sqrt(3) / 2.0, -R / 2.0]]

for i in range(n):
    x = random.randint(1, 100)
    y = random.randint(1, 100)
    s1 = round(math.sqrt((x - sensor[0][0])**2 + (y - sensor[0][1])**2))
    s2 = round(math.sqrt((x - sensor[1][0])**2 + (y - sensor[1][1])**2))
    s3 = round(math.sqrt((x - sensor[2][0])**2 + (y - sensor[2][1])**2))
    dataFile.write(f"{s1},{s2},{s3},{x},{y}\n")

