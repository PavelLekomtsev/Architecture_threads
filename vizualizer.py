import matplotlib.pyplot as plt

# Parsing
threads = {}

with open("result.txt", 'r') as file:
    lines = file.readlines()
    
    for i in range(len(lines)):
        line = lines[i].strip()
        
        if line.startswith("THREADS:"):
            num_threads = int(line.split(':')[1].strip())
            threads[num_threads] = []
        elif line.startswith("Time is"):
            exec_time = float(line.split(" ")[2])
            threads[num_threads][-1][1] = exec_time
        elif line.startswith("StartX"):
            step = float(line.split("Step:")[1].strip())
            threads[num_threads].append([step, 0])
        elif line.startswith("Function:"):
            name_figure = line.split(" ")[1]


# Painting
plt.figure(name_figure)
plt.title("Dependence of Time on Threads")
plt.xlabel("Negative shift degree")
plt.ylabel("Time, seconds")

colors = ["red", "blue", "green", "orange", "purple", "brown"]
count = 0

for key, values in threads.items():
    x = [m for m in range(len(values))]
    y = [point[1] for point in values]
    if count == 0:
        plt.plot(x, y, color = colors[count], label = str(key) + " Thread")
    else:
        plt.plot(x, y, color = colors[count], label = str(key) + " Threads")
    count += 1

plt.legend()
plt.grid()
# plt.show()
plt.savefig(f"Graphics/{name_figure}.png")