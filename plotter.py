import matplotlib.pyplot as plt
import csv, sys, re, random, os, time

##data reading section
datafile = sys.argv[1]
Mcsv = csv.reader(open(datafile, 'r'), delimiter=',', quotechar='"')

I = {}
M = []

##reading the first line and saving the header information

##reading the rest of the csv data and putting them to M matrix
index=0
for row in Mcsv:
	if index==0:
		for i in range(len(row)):
			I[row[i]] = i
	else:
		M.append(row)
	index+=1

print(I)
print(M)

##########Plot configuration section
plt.figure(figsize=(10,8))
#########Plotting section
user_col = list(map(lambda r: r[I['user_count']], M))
throughput_col = list(map(lambda r: float(r[I['avg_throughput']]), M))
response_col = list(map(lambda r: float(r[I['avg_response_time']]), M))

plt.xticks(rotation=90, fontsize = 'xx-small')

plt.plot(user_col, throughput_col, label='throughput', ls='-', color='red', marker='x', markersize=9, mew=2, linewidth=2)
plt.xlabel('User count')
plt.ylabel('Throughput (req/s)')
plt.grid(b='on')
plt.legend(loc=0)
#plt.show()
plt.savefig("output1.png")
plt.close()

plt.xticks(rotation=90, fontsize = 'xx-small')
plt.plot(user_col, response_col, label='response', ls='-', color='red', marker='x', markersize=9, mew=2, linewidth=2)
plt.xlabel('User count')
plt.ylabel('response time(per request)')
plt.grid(b='on')
plt.legend(loc=0)
plt.savefig("output2.png")
plt.close()
