#!/bin/bash

echo "Setting up Blazr..."
git submodule init
git submodule update
premake5 gmake
premake5 vs2022
make
