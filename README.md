# FCM-Sketch: Generic Network Measurements with Data Plane Support

## Software simulator for accuracy evaluation

This repository includes the c++ implementation of the CoNEXT 2020 paper [*FCM-Sketch: Generic Network Measurements with Data Plane Support*](https://www.comp.nus.edu.sg/~songch/papers/conext20_fcmsketch.pdf).

### Measurement
For each measurement task, we evaluate FCM-Sketch compared to many other algorithms:
- Data-plane queries:
    - `Flow size estimation`: CM-Sketch, CU-Sketch (Conservative-Update with Count-Min), PCM-Sketch (PyramidSketch with Count-Min), ElasticP4 (P4-version of ElasticSketch)
    - `Heavy hitter detection`: Hashpipe, UnivMon, ElasticP4 
    - `Cardinality`: Hyperloglog, UnivMon, ElasticP4
- Control-plane queries:
    - `Flow size distribution`: MRAC, ElasticP4
    - `Entropy`: MRAC, UnivMon, ElasticP4

We skip to show the `Heavy change detection` as its performance is very close to `Heavy hitter detection`. You can see *Section 4.4* how to get the heavy changes using FCM-Sketch. Also, see *Section 7.2* for the details of evaluation metrics (ARE, AAE, RE, WMRE, F1-Score).
We use the existing public source codes for this simulator. For example, we mainly refer the [ElasticSketch github repository](https://github.com/BlockLiu/ElasticSketchCode/tree/master/src/CPU).

Note that this code is for accuracy measurement. We do not intend to measure a running time (i.e., throughput on software switch). 

### How to compile & evaluate

We use `gcc+-5` to compile the cpp source codes. All optionals can be found in `Makefile`.

First, you can either compile each or all algorithms in this directory using `make`. For example,

- `make` : it compiles all algorithms.
- `make [Solution].out` : it compiles a specific algorithm named `[Solution].cpp` in this directory. 
For example, to compile FCM-Sketch, use `make fcmsketch.out`.
- `make clean` : it cleans all `*.out` files.

To run the evaluation, you can simply run `./[Solution].out` which shows the results in terms of evaluation metrics (e.g., ARE, AAE, ...).

### Sample trace

When you run the `./[Solution].out`, it automatically downloads a sample trace having ~260 MB .dat file. Refer the source code `src/common_func.h`.

### To do list

Currently, our FCM-Sketch implementation is only for 3-level trees (8, 16, and 32-bit counters). In future, we will generalize the code to arbitrary levels.

## Citing FCM-Sketch

You can cite this repository or FCM-Sketch as

    @article{TBD}
