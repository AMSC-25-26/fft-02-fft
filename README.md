# Fast Fourier Transform (FFT) Implementations in C++

This project provides implementations of the Fast Fourier Transform (FFT) in C++, including **iterative**, **recursive**, and **parallel MPI versions**. It allows you to process input data from text files, compute both FFT and inverse FFT (IFFT), and measure execution time. Results can be visualized using a Python plotting script.

---

## Requirements

- **C++17** compatible compiler (e.g., `g++` or `mpic++`)
- **MPI library** (e.g., OpenMPI)
- **OpenMP** (for parallel optimizations)
- **Python 3.x** (for plotting)
- **Matplotlib** (Python package)

### Install dependencies on Ubuntu:

```bash
sudo apt-get install build-essential libopenmpi-dev python3 python3-pip
pip3 install matplotlib
```

---

## Build Instructions

The project is built using the provided Makefile. Compilation requires an MPI-enabled C++ compiler (`mpic++`) and OpenMP support.

From the project root directory, run:

```bash
make
```

This command will:
- Compile the main FFT executable (`main`) using `mpic++`
- Compile the input generator (`gen`) using `g++`
- Automatically handle dependencies

---

## Generating Input Data

Before running the FFT program, you need to generate a text file containing the input samples.

To generate the input file:

```bash
make generate
```

This will:
- Execute the input generator from `src/utilities/`
- Interactively prompt you for:
  - A mathematical function of `x` (e.g., `sin(2*pi*x)`)
  - Domain start (e.g., `-2`)
  - Domain end (e.g., `2`)
- Generate the file: **`src/gen.txt`**

This file is used automatically by all run targets.

---

## Running the FFT Program

The FFT program can be executed using predefined Makefile targets, each corresponding to a different FFT method.

### Available Run Targets

| Target | Description |
|--------|-------------|
| `make run-iterative` | Run the iterative FFT |
| `make run-recursive` | Run the recursive FFT |
| `make run-parallel` | Run the parallel MPI FFT (4 processes by default) |
| `make run-parallel NP=8` | Run the parallel MPI FFT with 8 processes |
| `make run-all` | Run all methods sequentially (4 processes by default) |
| `make run-all NP=2` | Run all methods with 2 processes |

### Example Usage

```bash
make run-iterative
```

### Running with MPI (Multiple Processes)

The `run-parallel` and `run-all` targets automatically use `mpirun` with 4 processes by default. You can customize the number of processes using the `NP` variable:

```bash
make run-parallel NP=8    # Run with 8 MPI processes
make run-all NP=2         # Run all methods with 2 MPI processes
```

Alternatively, you can run manually:

```bash
mpirun -np 4 ./main 3 src/gen.txt
```

Where:
- `-np 4` specifies 4 processes
- `3` is the method code for parallel FFT
- `src/gen.txt` is the input file

---

## Output Files

The program computes both the forward FFT and the inverse FFT (IFFT). For each method, the following output files are generated:

- **Forward FFT results:** `output_<method>.txt`
- **Inverse FFT (IFFT) results:** `output_<method>_IFFT.txt`

### Example

Running the Iterative method will generate:
- `output_Iterative.txt` — Forward FFT results
- `output_Iterative_IFFT.txt` — Inverse FFT results

---

## Plotting Results

Results can be visualized using the provided Python script.

### Plot a Specific File

```bash
make plot FILE=output_Iterative.txt
```

### Predefined Plot Targets

```bash
make plot-iterative    # Plot iterative FFT results
make plot-recursive    # Plot recursive FFT results
make plot-parallel     # Plot parallel FFT results
```

Each command generates a `.png` plot using matplotlib.

---

## Cleaning the Project

### Remove Object Files and Dependencies

```bash
make clean
```

This removes:
- Object files (`*.o`)
- Dependency file (`make.dep`)

### Remove Everything (Full Clean)

```bash
make distclean
```

This removes:
- All object files and dependencies
- Executables (`main`, `gen`)
- All output files (`output_*.txt`)
- All generated plots (`*.png`)

---

## Directory Structure

```
.
├── Makefile
├── README.md
├── src/
│   ├── main.cpp
│   ├── libraries/
│   │   └── (FFT implementation headers)
│   ├── utilities/
│   │   └── GenerateInput.cpp
│   ├── plot_results.py
│   └── gen.txt (generated)
└── Documents/
```

---

## Example Workflow

### 1. Compile the Project

```bash
make
```

### 2. Generate Input Data

```bash
make generate
```

When prompted, enter:
```
Insert a function of x: sin(2*pi*x)
Enter domain start: -2
Enter domain end: 2
```

### 3. Run FFT

Choose one of the following:

```bash
make run-iterative
make run-recursive
make run-parallel
make run-all
```

### 4. Plot Results

```bash
make plot-iterative
```

This generates a plot showing both the forward FFT and inverse FFT results.

---

## Using Audio Files as Input (converter.py)

Instead of generating synthetic input data with the input generator, you can convert a real audio file into a text format compatible with `main.cpp` using the Python script `src/converter.py`.

### 1. Install Python Dependencies

The converter relies on `librosa` and `numpy`. Install them (preferably in a virtual environment):

```bash
pip install librosa numpy
```

### 2. Convert an Audio File to Text

From the project root directory, run:

```bash
python3 src/converter.py path/to/audio.m4a -o src/gen.txt
```

Where:
- `path/to/audio.m4a` is your input audio file (mono will be used)
- `-o src/gen.txt` saves the extracted samples into `src/gen.txt`, which is the default input file used by `main.cpp`

Optionally, you can limit the duration (in seconds) of the audio segment to convert:

```bash
python3 src/converter.py path/to/audio.m4a -o src/gen.txt -s 5.0
```

This will convert only the first 5 seconds of the audio.

### 3. Run the FFT on the Audio Data

Once `src/gen.txt` has been created by `converter.py`, you can run any FFT method exactly as with generated data. For example:

```bash
make run-iterative
```

The program will treat the audio samples in `src/gen.txt` as the input signal and compute its frequency-domain representation.

---

## Method Codes

When running the program manually, use these method codes:

| Code | Method |
|------|--------|
| `1` | Iterative FFT |
| `2` | Recursive FFT |
| `3` | Parallel MPI FFT |
| `4` | Run all methods |

### Example

```bash
./main 2 src/gen.txt    # Run recursive FFT
```

---

## Notes

- The input size is automatically padded to the nearest power of 2 if necessary
- Execution times are reported in milliseconds
- The parallel version uses both MPI and OpenMP for maximum performance
- All methods produce identical mathematical results (verified by IFFT)

---

## Troubleshooting

### "Could not open file" Error

Make sure you've generated the input file:
```bash
make generate
```

### MPI Warnings During Compilation

The warnings about function type casting from OpenMPI headers are harmless and can be ignored.

### Empty Output Data

Regenerate the input file:
```bash
make clean
make
make generate
```

---

For more details, see the source code in the `src/` and `src/libraries/` directories.