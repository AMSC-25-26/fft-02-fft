# Fast Fourier Transform (FFT) Implementations in C++

This project provides implementations of the Fast Fourier Transform (FFT) in C++, including **iterative, recursive, and parallel MPI versions**. It allows you to process input data from text files, compute both FFT and inverse FFT (IFFT), and measure execution time. Results can be visualized using a Python plotting script.
---

## Build Instructions

Compile the project using `mpic++`:

```sh
mpic++ -std=c++17 -Wall -Wextra -Wno-cast-function-type -I. main.cpp -o main
````

**Notes:**

* You may see warnings like:

  ```
  libraries/Parallel.hpp:40: warning: ignoring '#pragma omp parallel' [-Wunknown-pragmas]
  ```

  This is normal if OpenMP is not fully supported by your compiler.
* The output executable will be named `main`.

---

## Run Instructions

After building, run the executable with:

```sh
./main <method> <input_file>
```

### Command-line Arguments

1. `<method>` — FFT method selection:

   * `1` : Iterative FFT
   * `2` : Recursive FFT
   * `3` : Parallel MPI FFT
   * `4` : Run all methods for comparison

2. `<input_file>` — Path to a text file containing the input data.

---

## Generating Input Data

Before running the FFT program, you need to generate a text file containing the input samples. Use the provided `GenerateInput.cpp` in the `utilities/` folder.

### Steps:

1. Go to the `utilities/` folder:

```sh
cd src/utilities/
```

2. Compile the input generator:

```sh
g++ GenerateInput.cpp -o gen
```

3. Run the generator:

```sh
./gen
```

4. Follow the prompts:

* **Insert a function of x:**  
  Example: `sin(2*pi*x)` or `cos(pi*0.25*x)`

* **Enter domain start:**  
  Example: `-2`

* **Enter domain end:**  
  Example: `2`

The generator will validate the domain and save the samples to:

```sh
../gen.txt
```

This file (`gen.txt`) can now be used as the `<input_file>` argument when running `main`.

**Example usage:**

```sh
cd ../../   # go back to project root
./main 1 gen.txt
```
---

## Output

The program computes both the forward FFT and the inverse FFT (IFFT). For each method, the following output files are generated:

* **Forward FFT results:** `output_<method>.txt`
* **Inverse FFT (IFFT) results:** `output_<method>_IFFT.txt`

**Example:** Running the Iterative method will generate:

* `output_Iterative.txt` — Forward FFT results
* `output_Iterative_IFFT.txt` — Inverse FFT results


## Plotting Results

The results of the FFT executions are saved to `output.txt`. You can visualize the timing results using the Python script:

```sh
python3 plot_results.py output.txt
```

---

## Directory Structure

* `src/` — Source files, including:
    * `main.cpp` — Main executable.
    * `libraries/` — Header and implementation files for different FFT methods.
    * `utilities/` — C++ files for generating input data.
    * `plot_results.py` — Python script for visualizing results.
* `Documents/` — Project documentation.

---

## Requirements

* C++17 compatible compiler (e.g., `g++` or `mpic++`)
* MPI library (e.g., OpenMPI)
* Python 3.x (for plotting)
* Matplotlib (Python package)
* OpenMP (optional, for parallel FFT)

Install dependencies on Ubuntu:

```sh
sudo apt-get install build-essential libopenmpi-dev python3 python3-pip
```

---

## Example Workflow

**1. Compile:**

```sh
mpic++ -std=c++17 -Wall -Wextra -Wno-cast-function-type -I. main.cpp -o main
```

**2. Run FFT:**

```sh
./main 1 gen.txt
```

**3. Plot results:**

```sh
python3 plot_results.py output.txt
```

---

For more details, see the source code in the `src/` and `libraries/` directories.
