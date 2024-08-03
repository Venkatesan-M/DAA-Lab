import matplotlib.pyplot as plt
import seaborn as sns

# Data
sizes = [10, 100, 500, 1000, 5000, 10000, 50000, 100000, 500000, 1000000]
T_n = [33.2193, 664.386, 4482.89, 9965.78, 61438.6, 132877, 780482, 1.66096e+06, 9.46578e+06, 1.99316e+07]
t_n = [2e-06, 1e-05, 5.6e-05, 0.000103, 0.000567, 0.001194, 0.006375, 0.013109, 0.071423, 0.143031]

# Set the style and context for the plot
sns.set(style="whitegrid", context="notebook")

# Plot Size vs T(n)
plt.figure(figsize=(10, 6))
sns.lineplot(x=sizes, y=T_n, marker='o', color='blue')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Size (n)')
plt.ylabel('T(n)')
plt.title('Size vs T(n)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.savefig('size_vs_Tn.png', dpi=300)
plt.close()

# Plot Size vs t(n)
plt.figure(figsize=(10, 6))
sns.lineplot(x=sizes, y=t_n, marker='o', color='green')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('Size (n)')
plt.ylabel('t(n) (seconds)')
plt.title('Size vs t(n)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.savefig('size_vs_tn.png', dpi=300)
plt.close()

# Plot T(n) vs t(n)
plt.figure(figsize=(10, 6))
sns.lineplot(x=T_n, y=t_n, marker='o', color='red')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('T(n)')
plt.ylabel('t(n) (seconds)')
plt.title('T(n) vs t(n)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.savefig('Tn_vs_tn.png', dpi=300)
plt.close()

print("Graphs have been saved as 'size_vs_Tn.png', 'size_vs_tn.png', and 'Tn_vs_tn.png'")
