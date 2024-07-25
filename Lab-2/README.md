# DAA-Lab - Lab 1

## Overview

This project contains C++ code for Implementation Insertion Sort with experiments on the analysis and the efficiency.

## Project Structure


- `code.cpp`: Contains the C++ implementation for finding the minimum and maximum scores.
- `time.cpp`: Contains the C++ implementation for timing the execution.
<!-- - `plot.py`: Python script to generate and save graphs of execution time versus the number of participants.
- `execution_time_vs_participants.png`: Default PNG file for the execution time graph.
- `execution_time_vs_participants_dark.png`: Dark-themed PNG file for the execution time graph. -->

## Prerequisites

Ensure you have the following installed:

- C++ compiler (e.g., `g++`)
- Python 3.x
- Python libraries: `matplotlib`, `seaborn`

## Instructions

### 1. Compile and Run the C++ Code

To compile and execute the C++ programs, follow these steps:

1. **Compile `code.cpp`**:
    ```sh
    g++ -o code code.cpp
    ```

2. **Run the executable**:
    ```sh
    ./code
    ```

   This program will prompt for the number of participants and their scores and display the minimum and maximum scores along with the execution time.

3. **Compile `time.cpp`** (if needed):
    ```sh
    g++ -o time time.cpp
    ```

4. **Run the executable**:
    ```sh
    ./time
    ```

   This program will output the execution time for different numbers of participants.

### 2. Generate Execution Time Graph

To generate and save the execution time graph, use the Python script `plot.py`:

1. **Ensure Python libraries are installed**:
    ```sh
    pip install matplotlib seaborn
    ```

2. **Run the Python script**:
    ```sh
    python plot.py
    ```

   This will generate and save the execution time graphs as PNG files: `execution_time_vs_participants.png` and `execution_time_vs_participants_dark.png`.

## Results

- `execution_time_vs_participants.png`: Standard graph with default styling.
- `execution_time_vs_participants_dark.png`: Dark-themed graph for better visualization in dark mode.

