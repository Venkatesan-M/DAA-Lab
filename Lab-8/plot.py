import matplotlib.pyplot as plt

# Read the data from the file
input_sizes = []
times = []

with open("huffman_times.txt", "r") as file:
    for line in file:
        size, time = line.split()
        input_sizes.append(int(size))
        times.append(float(time))

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(input_sizes, times, marker='o', linestyle='-', color='b')
plt.title('Time Complexity of Huffman Encoding')
plt.xlabel('Input Size (number of letters)')
plt.ylabel('Time Taken (seconds)')
plt.grid(True)

# Save the plot as a PNG file
plt.savefig("huffman_time_complexity.png")

# Show the plot
plt.show()