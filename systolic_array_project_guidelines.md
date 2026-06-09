# TPU-Like Systolic Array Simulator Project Guidelines

## Project Goal

Build a C++ simulator for a configurable 2D systolic array that runs tiled GEMM workloads:

```text
C = A x B
```

The simulator should help answer:

```text
For an M x K matrix A and a K x N matrix B,
running on an R x C processing-element array,
how many cycles does the workload take,
how much PE utilization is achieved,
and how much data movement is required?
```

This project is meant to demonstrate AI accelerator architecture knowledge, C++ implementation skill, and verification discipline.

## Learning Objectives

By the end of the project, you should be able to explain and implement:

- GEMM mapping onto a 2D processing-element array.
- Output-stationary systolic array dataflow.
- PE-level multiply-accumulate behavior.
- Array fill and drain latency.
- Tiling over M and N dimensions.
- Edge-tile handling when matrix dimensions do not divide evenly by array dimensions.
- Golden-model verification.
- Performance counters such as cycles, useful MACs, peak MAC capacity, and utilization.
- Basic memory traffic analysis for A, B, and C tensors.

## Core Architecture

The first version should use an output-stationary dataflow.

In output-stationary mode:

- Each PE owns one output element within the current output tile.
- A values move horizontally across the array.
- B values move vertically down the array.
- Partial sums stay inside the PE until the output element is complete.

For an array with `array_rows = R` and `array_cols = C`, one tile computes up to:

```text
R x C output elements
```

For a GEMM:

```text
A: M x K
B: K x N
C: M x N
```

One output tile uses:

```text
A_tile: active_rows x K
B_tile: K x active_cols
C_tile: active_rows x active_cols
```

Where:

```text
active_rows <= R
active_cols <= C
```

Edge tiles may have fewer active rows or columns than the physical array.

## Required Milestones

### Milestone 1: Golden GEMM

Implement a simple CPU reference GEMM.

Requirements:

- Matrix container using standard C++ data structures.
- Random matrix generation.
- Naive golden GEMM.
- Result comparison.
- Small deterministic tests.

Suggested test cases:

- 1 x 1 matrices.
- 2 x 2 matrices.
- Non-square matrices.
- `K = 1`.
- Matrix dimensions smaller than the array.
- Dimensions not divisible by the array size.
- Random small matrices.
- Zero matrices.

### Milestone 2: Tiled GEMM Mapping

Implement tiled output computation.

Requirements:

- Tile across the M dimension by `array_rows`.
- Tile across the N dimension by `array_cols`.
- Accumulate across the full K dimension.
- Correctly handle edge tiles.
- Match golden GEMM for all tested dimensions.

### Milestone 3: Analytical Cycle Model

For each output tile, estimate cycles using:

```text
tile_cycles ~= K + active_rows + active_cols - 2
```

This formula models:

- K cycles of useful dot-product work.
- Additional cycles for wavefront fill and drain through the PE array.

Track:

```text
useful_macs = active_rows * active_cols * K
physical_peak_macs_for_tile = tile_cycles * array_rows * array_cols
tile_utilization = useful_macs / physical_peak_macs_for_tile
```

For the full workload:

```text
total_useful_macs = M * N * K
total_peak_macs = total_cycles * array_rows * array_cols
overall_utilization = total_useful_macs / total_peak_macs
```

Important distinction:

- `active_rows` and `active_cols` describe the useful tile work.
- `array_rows` and `array_cols` describe the physical hardware capacity.

For edge tiles, some physical PEs may be idle.

### Milestone 4: Explicit PE-Level Tile Simulation

Build an explicit cycle-by-cycle simulator for one tile.

Each PE should track:

- Current A operand.
- Current B operand.
- Partial sum.
- Whether it performed a useful MAC in the current cycle.
- Total active cycles.

Each cycle:

- Inject A values from the left side.
- Inject B values from the top side.
- Move A values right.
- Move B values down.
- Perform a MAC when a valid A and valid B with matching K index meet at a PE.

The explicit simulator should produce the same tile result as the golden reference.

### Milestone 5: Memory Traffic Model

Report idealized memory traffic.

At minimum:

```text
A_bytes_read = M * K * bytes_per_element
B_bytes_read = K * N * bytes_per_element
C_bytes_written = M * N * bytes_per_output
total_bytes = A_bytes_read + B_bytes_read + C_bytes_written
macs_per_byte = total_useful_macs / total_bytes
```

Later, refine this to account for tiled reloads and limited SRAM capacity.

### Milestone 6: Experiments

Run sweeps across:

- Array sizes: 2x3, 4x4, 8x8, 16x16, 32x32.
- Matrix sizes: small, medium, large.
- K values: 1, 4, 16, 64, 256, 1024.
- Divisible and non-divisible dimensions.

Answer:

- When is utilization high?
- When does fill/drain overhead dominate?
- How much do edge tiles hurt?
- How does larger array size affect cycle count?
- When does memory traffic become the bottleneck?

## Suggested Command-Line Interface

Example:

```powershell
.\systolic_sim.exe --M 64 --N 96 --K 128 --array-rows 8 --array-cols 16 --seed 1
```

Suggested output:

```text
Correct: yes
M: 64
N: 96
K: 128
Array: 8 x 16
Total useful MACs: ...
Total cycles: ...
Peak MACs/cycle: ...
Overall utilization: ...
A bytes read: ...
B bytes read: ...
C bytes written: ...
MACs per byte: ...
```

## Suggested Repository Structure

```text
systolic-sim/
  CMakeLists.txt
  README.md
  include/
    matrix.hpp
    systolic_array.hpp
    simulator_config.hpp
    stats.hpp
  src/
    main.cpp
    matrix.cpp
    golden_gemm.cpp
    systolic_array.cpp
    stats.cpp
  tests/
    test_gemm.cpp
    test_tiling.cpp
    test_systolic_array.cpp
  experiments/
    README.md
    results/
```

Keep the first implementation simple. Add structure only when the code starts asking for it.

## Coding Guidelines

Use C++17 or C++20.

Prefer:

- `std::vector`.
- Plain structs for configuration and stats.
- Clear nested loops.
- Small functions with specific responsibilities.
- Integer inputs first, such as `int8_t` or `int`.
- `int32_t` accumulation for integer GEMM.

Avoid at first:

- External matrix libraries.
- GPU APIs.
- Heavy template metaprogramming.
- Overly abstract class hierarchies.
- Premature optimization.

## Environment Setup

Recommended Windows setup:

- Git.
- CMake.
- Ninja.
- Visual Studio Build Tools 2022 with "Desktop development with C++".
- VS Code with the C/C++ extension, optional.

Check installation:

```powershell
git --version
cmake --version
ninja --version
```

From a Visual Studio Developer PowerShell, check:

```powershell
cl
```

Suggested build flow:

```powershell
cmake -S . -B build -G Ninja
cmake --build build
.\build\systolic_sim.exe --M 64 --N 96 --K 128 --array-rows 8 --array-cols 16 --seed 1
```

Testing can start with a simple no-dependency executable:

```powershell
.\build\systolic_tests.exe
```

## Resume-Ready Definition

The project is resume-ready when it has:

- Configurable `M`, `N`, and `K`.
- Configurable physical array rows and columns.
- Golden GEMM verification.
- Tiled output-stationary GEMM execution.
- Cycle estimation.
- Utilization reporting.
- Memory traffic reporting.
- Documented experiments and observations.
- A README explaining the architecture and results.

## Stretch Goals

Add only after the core project is correct:

- Explicit cycle-by-cycle PE simulation for full GEMM.
- Weight-stationary dataflow comparison.
- SRAM capacity model.
- HBM bandwidth stall model.
- INT8 inputs with INT32 accumulation.
- CSV output for experiment sweeps.
- Transformer workload presets, such as Q/K/V projection GEMMs.
- Roofline-style compute-bound versus memory-bound classification.

## Interview Explanation Template

Use this as a starting point:

```text
I built a C++ simulator for a configurable TPU-style systolic array running tiled GEMM workloads. The simulator models output-stationary dataflow, where A operands stream horizontally, B operands stream vertically, and partial sums remain local inside each PE. I verified the simulator against a golden CPU GEMM and added performance counters for total cycles, useful MACs, PE utilization, and memory traffic. I used the simulator to study how array size, tile shape, K dimension, and edge tiles affect accelerator utilization.
```

## Key Questions To Be Able To Answer

- Why is GEMM central to AI accelerators?
- What does each processing element do?
- What does output-stationary mean?
- How do A and B operands move through the array?
- Why is utilization less than 100 percent?
- Why does a larger K dimension usually improve utilization?
- How do edge tiles waste physical PE capacity?
- What is the difference between functional correctness and cycle modeling?
- How does memory traffic limit accelerator performance?

