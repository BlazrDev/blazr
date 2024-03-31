#!/bin/bash

echo "Setting up Blazr..."
premake5 gmake
premake5 vs2022
make
