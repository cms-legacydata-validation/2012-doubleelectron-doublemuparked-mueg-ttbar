#!/bin/bash

# compile code (produces two executables)
g++ ttbarMakeHist.cxx -o ttbarMakeHist `root-config --cflags --libs` -lMathMore -std=c++11
g++ ttbarMakePlots.cxx -o ttbarMakePlots `root-config --cflags --libs` -std=c++11

# create needed directories if do not exist yet
mkdir -p data mc hist plots
