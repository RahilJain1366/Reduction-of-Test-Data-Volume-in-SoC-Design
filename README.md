# Dictionary-Based Test Data Compression for SoC Design

This C++ project implements a solution based on the paper  
**"Test data compression using dictionaries with selective entries and fixed-length indices"**  
by N. Ahmed, M. Tehranipoor, and J. Rajski.

The program addresses the **clique partitioning problem** for dictionary-based test data compression, specifically designed to optimize test data volume in **System-on-Chip (SoC)** circuit testing.

---

## 📌 Overview

- **Objective**: Minimize test data volume using dictionary-based compression.
- **Methodology**:
  - Model test patterns as a graph.
  - Apply a heuristic algorithm for clique partitioning.
  - Select dictionary entries that yield optimal compression with fixed-length indices.

---

## ⚙️ Features

- Graph-based modeling of test data.
- Heuristic clique partitioning algorithm.
- Selective dictionary generation and evaluation.
- Outputs compressed test data and compression statistics.

---

## 🧱 Dependencies

- C++11 or higher
- Standard Template Library (STL)
- No external libraries required

---

📖 Reference
N. Ahmed, M. Tehranipoor, and J. Rajski,
"Test data compression using dictionaries with selective entries and fixed-length indices,"
IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems,
vol. 26, no. 2, pp. 279–287, Feb. 2007.

## 🛠️ Build Instructions

```bash
g++ -std=c++11 -o test_compression main.cpp
./test_compression s15850f.test 4 32 s15850f.dict


