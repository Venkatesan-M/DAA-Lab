import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

# Data preparation
data = {
    'Size': [10, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000],
    'Numerical Merge Sort': [3e-06, 9e-06, 4.5e-05, 9.6e-05, 0.000562, 0.001243, 0.005976, 0.012534, 0.067926, 0.135117],
    'Alpha Merge Sort': [9.28e-06, 5.857e-05, 0.00037707, 0.00098953, 0.00510676, 0.00919159, 0.05398, 0.110983, 0.657262, 1.3299]
}

df = pd.DataFrame(data)

# Plotting
plt.figure(figsize=(12, 6))
sns.lineplot(data=df, x='Size', y='Numerical Merge Sort', label='Numerical Merge Sort', marker='o')
sns.lineplot(data=df, x='Size', y='Alpha Merge Sort', label='Alpha Merge Sort', marker='o')

# Customize the plot
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Size')
plt.ylabel('Time (seconds)')
plt.title('Comparison of Numerical Merge Sort and Alpha Merge Sort')
plt.legend()
plt.grid(True)

# Save the plot
plt.savefig('merge_sort_comparison.png')

# Show the plot (optional)
plt.show()
