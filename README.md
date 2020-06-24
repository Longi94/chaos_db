# Chaos Engineering for Databases

Inspired by [Chaos Monkey](https://github.com/netflix/chaosmonkey), this work focuses on injection main memory faults into databases and observing their effects. The repository includes a generic bit-flip injection tool that works on any linux platform that has the ptrace system call. As well as various python scripts to run a large experiment on a database locally or on a cluster using clush. The `database` folder contains various scripts for installing and setting up databases for running TPC-H queries on them.

The injector is written in C++, simply build it with `make`
