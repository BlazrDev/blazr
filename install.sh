#!/bin/bash

echo "Setting up Blazr..."
mkdir $(pwd)/bin/Debug-linux-x86_64/Sandbox
premake5 gmake
make
