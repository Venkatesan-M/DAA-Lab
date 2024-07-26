import matplotlib.pyplot as plt
import seaborn as sns

# Data from the output of time.cpp
input_sizes = [10, 100, 1000, 10000, 100000, 1000000]
execution_times = [1e-06, 7e-06, 0.00054, 0.048506, 4.54818, 459.508]

# Plotting the graph
sns.set(style="whitegrid")
plt.figure(figsize=(12, 8))
plt.plot(input_sizes, execution_times, marker='o', linestyle='-', color='b', label='Insertion Sort')

# Annotate time complexity
plt.text(20, 1e-06, 'O(n)', horizontalalignment='left', size='medium', color='green', weight='semibold')
plt.text(1e4, 1, 'O(n^2)', horizontalalignment='left', size='medium', color='red', weight='semibold')

# Label points on the plot
for i in range(len(input_sizes)):
    plt.text(input_sizes[i], execution_times[i], f"({input_sizes[i]},{execution_times[i]:.2e})", fontsize=9)

plt.xscale('log')
plt.yscale('log')
plt.xlabel('Input Size (n)')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time vs Input Size for Insertion Sort')
plt.legend()
plt.grid(True, which="both", ls="--")

# Save the plot
plt.savefig('execution_time_vs_input_size_enhanced.png', dpi=300, bbox_inches='tight')

# Show the plot
plt.show()
