C++14 LIBRARY FOR GENETIC ALGORITHMS PROGRAMMING
==============================================
Development is in progress.

Currently implemented features:
- Flexible definition of genotype models with ability to use custom data and set up parameters for each gene individually.
- Customizable mutation and crossover operators which behave accordingly to the defined genotype model. Library includes one-point crossover operator and random value mutation and shift operators.
- All standard genetic algorithm parameters, like the breaking conditions, controlled population cap, fitness function definition.
- Selection is based on ranking groups to reduce the chance of getting into local extremum and keep diversity. The cutoff curve can be manually defined.
- Customizable logging and statistics facilities.

## Installation

This is headers only library, so just copy into your project folder to get it working. Also CMake script is provided for classic installation as well as for development.

## Usage

See examples in source directory (the API layer is in development, so things will change to conceal many unnecessary details).
