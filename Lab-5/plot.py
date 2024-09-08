import matplotlib.pyplot as plt

# Read data from the file
sizes = []
times = []

with open('matrix_chain_times.txt', 'r') as file:
    for line in file:
        size, time = map(int, line.split())
        sizes.append(size)
        times.append(time)

# Plotting the results
plt.plot(sizes, times, marker='o')
plt.xscale('log')  # Use logarithmic scale for the x-axis
plt.yscale('log')  # Use logarithmic scale for the y-axis
plt.title('Matrix-Chain Multiplication Time Complexity')
plt.xlabel('Number of Matrices (n)')
plt.ylabel('Time (microseconds)')
plt.grid(True)
plt.savefig('matrix_chain_time_complexity.png')
plt.show()
