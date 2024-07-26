# DAA-Lab - Lab 2
This project involves implementing the Insertion Sort algorithm, analyzing its time complexity, and generating graphs to illustrate the relationship between the input size and the execution time.

## Project Structure
`code.cpp`: Contains the C++ implementation for the Insertion Sort algorithm.

`time.cpp`: Contains the C++ implementation for measuring execution time.

`plot.py`: Python script to generate and save graphs of execution time versus input size.

`execution_time_vs_input_size.png`: Graph of execution time versus input size.

`execution_time_vs_input_size_dark.png`: Dark-themed graph of execution time versus input size.


## Prerequisites
Ensure you have the following installed:

- C++ compiler (e.g., g++)
- Python 3.x
- Python libraries: matplotlib, seaborn
## Instructions
### 1. Compile and Run the C++ Code
To compile and execute the C++ programs, follow these steps:

```sh
g++ -o code code.cpp
```
Run the executable:

```sh
./code
```
Compile time.cpp:


```sh
g++ -o time time.cpp
```
Run the executable:

```sh
./time
```
This program will output the execution time for different input sizes.

### 2. Generate Execution Time Graph
To generate and save the execution time graph, use the Python script plot_insertion_sort.py:

Ensure Python libraries are installed:

```sh
pip install matplotlib seaborn
```
Run the Python script:

```sh
python plot.py
```
This will generate and save the execution time graphs as PNG files: `execution_time_vs_input_size.png` and `execution_time_vs_input_size_dark.png`.

## Results
`execution_time_vs_input_size.png`: Standard graph with default styling.
`execution_time_vs_input_size_dark.png`: Dark-themed graph for better visualization in dark mode.