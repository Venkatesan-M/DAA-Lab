import matplotlib.pyplot as plt

# Function to read time data from file
def read_times(filename):
    sizes = []
    times = []
    
    with open(filename, 'r') as file:
        lines = file.readlines()
        for line in lines:
            # Extract the size and time from each line
            size, time = line.split(':')
            size = int(size.split('=')[1].strip())  # Extract size (N)
            time = int(time.strip().split()[0])  # Extract time in microseconds
            sizes.append(size)
            times.append(time)
    
    return sizes, times

# Read data from the nqueen_times.txt file
sizes, times = read_times('nqueen_times.txt')

# Plotting the data
plt.plot(sizes, times, marker='o', linestyle='-', color='b')
plt.title('N-Queens Problem Time Complexity')
plt.xlabel('Board Size (N x N)')
plt.ylabel('Time (microseconds)')
plt.grid(True)
plt.savefig('nqueen_time_complexity.png')  # Save the plot as PNG
plt.show()
