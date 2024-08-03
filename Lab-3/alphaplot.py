import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

# Data
sizes = [10, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000]
Tn = [33.2193, 664.386, 4482.89, 9965.78, 61438.6, 132877, 780482, 1.66096e+06, 9.46578e+06, 1.99316e+07]
tn = [9.23e-06, 8.8129e-05, 0.000425491, 0.00081765, 0.00481134, 0.00986566, 0.0757431, 0.133851, 0.778395, 1.52895]

# Convert Tn to log scale
log_Tn = np.log2(Tn)

# Initialize plot
plt.figure(figsize=(12, 8))
sns.set(style='whitegrid')

# Plot T(n)
sns.lineplot(x=sizes, y=log_Tn, label='T(n) = n log(n)', marker='o')

# Plot t(n)
sns.lineplot(x=sizes, y=tn, label='t(n)', marker='o')

# Set plot labels and title
plt.xlabel('Input Size (n)')
plt.ylabel('Time (seconds)')
plt.title('Time Complexity Analysis of Merge Sort Algorithm for Strings')
plt.xscale('log')
plt.yscale('log')
plt.legend()

# Save plot as image
plt.savefig('time_complexity_analysis.png')

# Show plot
plt.show()
