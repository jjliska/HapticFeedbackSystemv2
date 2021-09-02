import json
import matplotlib.pyplot as plt
from pymongo import MongoClient

#Getting the mongodb server
myclient = MongoClient("mongodb://localhost:27017/")
db = myclient["data"]
collection = db["data"]
obj = collection.find()

count = 0
totPressure=0
avPressureList=[]

#Parsing the json data into a dict object and then gathering a 2d array from the given string of the json
#This is then averaged against the size of the array and again against the average
for data in obj:
    averagePressure = 0
    dataString = data.get("x")
    if data.get("x") != "[object Undefined]":
        count+=1
        tempArray = dataString.split("_")
        for i in range(0,len(tempArray)):
            tempArray[i] = tempArray[i].replace(" ","-").split("-")
            for j in range(0, len(tempArray[i])):
                try:
                    averagePressure+=float(tempArray[i][j])
                except:
                    break
        averagePressure=averagePressure/float(len(tempArray)*len(tempArray[0]))
        avPressureList.append(averagePressure)
        totPressure+=averagePressure

totPressure=totPressure/float(count)
print("Total number of valid objects: %i" %count)
print("Average pressure across the grid: %f" %totPressure)

#Showing the graph
plt.plot(range(0,count),avPressureList)

plt.xlabel('interaction')
plt.ylabel('pressure')
plt.title('Pressure over time')
plt.show()
