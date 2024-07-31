#!/bin/bash
git submodule init

dir_path="bin/Debug-linux-x86_64/Sandbox"
if [ ! -e "$dir_path" ]; then
  mkdir -p "$dir_path"
  if [ $? -eq 0 ]; then
    echo "Could not create directory: /bin/Debug-linux-x86_64/Sandbox"
  fi
fi
echo "Setting up Blazr..."
git submodule update
premake5 gmake
premake5 vs2022
make
