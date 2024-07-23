import matplotlib.pyplot as plt
import seaborn as sns

# Data
participants = [10, 100, 1000, 10000, 100000]
execution_times = [3e-06, 4e-06, 2.4e-05, 0.000204, 0.001892]

# Set the style and context for the plot
sns.set(style="darkgrid", context="notebook")
plt.style.use("dark_background")

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(participants, execution_times, marker='o', linestyle='-', color='cyan')

# Set the labels and title
plt.xlabel('Number of Participants (N)')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time vs Number of Participants')
plt.grid(True, linestyle='--', alpha=0.5)

# Save the plot as a PNG file
plt.savefig('execution_time_vs_participants_dark.png', dpi=300)

print("Graph has been saved as 'execution_time_vs_participants_dark.png'")
