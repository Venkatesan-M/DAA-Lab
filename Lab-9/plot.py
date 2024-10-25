import matplotlib.pyplot as plt

# Read data from file
input_sizes = []
times = []

with open("rabin_karp_times.txt", "r") as file:
    for line in file:
        size, time = line.split()
        input_sizes.append(int(size))
        times.append(float(time))

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(input_sizes, times, marker='o', linestyle='-', color='b')
plt.title('Time Complexity of Rabin-Karp Algorithm')
plt.xlabel('Input Size (length of text)')
plt.ylabel('Time Taken (seconds)')
plt.grid(True)

# Save the plot as a PNG file
plt.savefig("rabin_karp_time_complexity.png")

# Show the plot
plt.show()
