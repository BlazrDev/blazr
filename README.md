<img src="https://github.com/user-attachments/assets/9beb1472-3474-4613-98dc-a2b4166ad130" alt="blazr-logo-white" width="400">

# Blazr


## NOTE: Blazr is still early in developement and thus may contain a lot of bugs and might lack some functionalities. It is capable of creating a simple 2D game. Our goal is to be able to support more features in the future.

Blazr is an open source 2D game engine with the goal of eventually being able to create complex 2D games with complex mechanics and features. In the far future, our goal is to expand it to 3D as well.

## Building
### Windows
Clone the repository.
```bash
git clone https://github.com/Chameleeon/blazr.git
cd blazr
```

After that run the following commands:

```bash
git submodule init
git submodule update
premake5.exe vs2022
```

After that is done, you can open the Blazr.sln file and compile the code from Visual Studio.

### Linux
If you are on Linux, run the following commands:
```bash
git clone https://github.com/Chameleeon/blazr.git
cd blazr
./install.sh
```

To run the Sandbox application, type:
```bash
./run.sh
```

# How To Use
For a detailed tutorial on how to use the Blazr engine, and on inforation how certain things are implemented, please refer to our [Wiki](https://github.com/BlazrDev/blazr/wiki).
