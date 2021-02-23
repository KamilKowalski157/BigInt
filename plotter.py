import matplotlib.pyplot as plt
file = open("Logs/karatsuba_log2.txt")
data_raw = file.read().split(",")
print(data_raw)
data = [round(float(i),2) for i  in data_raw]
print(data)
coeff = 0
weight = 0
for i in range(0,len(data)-1):
    coeff += (data[i+1]**2)/data[i]
    weight += data[i+1]
coeff/=weight
print("Coefficient: ",coeff)

plt.plot([2**i for i in range(0,len(data_raw))],data,"ro")
plt.show()
