import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# Data from the C++ program
data = {
    'Length': [10, 100, 1000, 10000, 50000],
    'Time (microseconds)': [36, 27, 2175, 214356, 5339872]
}

# Create a DataFrame
df = pd.DataFrame(data)

# Plot using Seaborn
plt.figure(figsize=(10, 6))
sns.lineplot(x='Length', y='Time (microseconds)', data=df, marker='o')
plt.yscale('log')  # Use logarithmic scale for better visualization
plt.xscale('log')  # Use logarithmic scale for better visualization
plt.xlabel('Rod Length')
plt.ylabel('Time (microseconds)')
plt.title('Time Complexity of Rod Cutting Problem (Tabulation Approach)')
plt.grid(True, which="both", ls="--", linewidth=0.5)

# Save the plot as an image
plt.savefig('time_complexity_tabulation_plot.png', dpi=300, bbox_inches='tight')

# Optionally, you can still display it
# plt.show()
