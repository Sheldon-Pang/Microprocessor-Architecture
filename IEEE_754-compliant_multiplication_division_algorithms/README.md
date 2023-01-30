# Microprocessor-Architecture

### Short Summary
Analyze and quantify the performance trade-offs of using native vs. emulated floating point numbers.

### Context

In embedded processing, it is quite common to deploy microcontrollers without hardware-based floating point support.  In these cases, programmers must make the choice to use emulated floating point (floating point support that is supplied through library code), or refactor their algorithms to use either fixed-point, or rational-based computations.  Emulated floating point is often undesirable for two reasons: it is significantly slower than the alternatives, and it takes considerably more instructions to implement (using up limited instruction memory).

### Objective

* Implement IEEE 754-compliant multiplication and division algorithms
* Quantify trade-offs between native and emulated floating point solutions.
* Measure performance of an actual program and estimate business impact of improving the performance.