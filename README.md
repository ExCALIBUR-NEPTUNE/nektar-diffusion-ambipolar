<div align="center">
  <a href="https://www.nektar.info/" target="_blank" >
    <img alt="Nektar++ diffusion" src="readme/icon-blue.png" width="100" />
  </a>
</div>
<div align="center">
  <a href="LICENSE.txt" target="_blank">
    <img alt="Software License" src="https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square">
  </a>
</div>

<h1>Nektar-diffusion proxy-app</h1>

## Table of contents

  * [Description](#description)
  * [Installation and dependencies](#installation-and-dependencies)
    * [Docker](#docker)
    * [Using Nektar++ binary packages](#using-nektar-binary-packages)
    * [Using Nektar++ source code](#using-nektar-source-code)
  * [Execution](#execution)
  * [Parameters](#parameters)
  * [References](#references)
  * [License](#license)

## Description
**Nektar-diffusion proxy-app**: An anisotropic thermal conduction proxy-app for the magnetized plasma written in Nektar++ framework [[1]](#cantwell-et-al-2015). The derivation of the anisotropic thermal conduction in the magnetized plasma and its variational formulation are documented in the `docs` folder. For the detailed formulations and tutorials of Nektar++, please refer to the documentation on the [Nektar++ website](https://www.nektar.info/). Some examples are provided in the `example` folder. 

The variational formulation of the two-dimensional anisotropic thermal conduction in the magnetized plasma can be written as

![Variational formulation](/readme/equation.jpg)

where $`\psi`$ and $`T`$ respectively are the test function and the temperature. $`\mathbf{\kappa}_c`$ is the anisotropic thermal conductivity tensor and $`Q`$ represents the heat source in field. $`\mathbf{n}`$ is the outward normal vector along the boundaries of the domain.


## Installation and dependencies
The Nektar-diffusion proxy-app should be compiled against Nektar++ v5.5.0. This can be done by either using pre-compiled binary packages or compiling it from source. Alternatively a Dockerfile image can be generated which includes Nektar++, the nektar-diffusion proxy-app and the included examples.

### Docker
To use the docker image, clone the nektar-diffusion repository and, from the top directory, build the image using
```bash
docker build -t nektar-diffusion .
```
and then run an interactive shell
```bash
docker run -it nektar-diffusion /bin/bash
```

### Using Nektar++ binary packages
Install CMake using your normal package management tools.

Install the v5.5.0 `libnektar++-dev` or `libnektar++-devel` package (as appropriate), following instructions at https://www.nektar.info.

Then compile the nektar-diffusion solver using
```bash
mkdir build && cd build
cmake ..
make install
```

### Using Nektar++ source code
Download the source code for Nektar++ v5.5.0
```bash
git clone https://gitlab.nektar.info/nektar/nektar
cd nektar
git checkout v5.5.0
```
Compile it following the instructions at [https://www.nektar.info](https://www.nektar.info). To save time, set NEKTAR_BUILD_SOLVERS=OFF and NEKTAR_BUILD_DEMOS=OFF. Many of the dependencies are available as pre-built packages in most Linux distributions (e.g. Boost, TinyXML, Scotch, BLAS, LAPACK). It is recommended to turn on the NEKTAR_USE_MPI option to enable parallel execution. Run `make install` to collate the binaries and library files under the `dist` sub-directory and (optionally) set CMAKE_INSTALL_PREFIX to your preferred install location. To check Nektar++ is built correctly run `ctest`.

Then compile the nektar-diffusion solver using
```bash
mkdir build && cd build
cmake -DNektar++_DIR=/path/to/build/dist/lib/nektar++/cmake -DNEKTAR_BUILD_DOCS=ON ..
make install
```
where `/path/to/build/dist/lib/nektar++/cmake` is the path containing the *Nektar++Config.cmake* file.

**Debugging compilation, installation and testing:**

If the compilation fails, check the possible broken links during the configuration by the command *ccmake ..* and toggle the curses interface to the advanced mode by press the *t* key. A list of detailed links to the libraries will appear for investigation.

If an error was observed during installation, take note the description of this error and identify which program causes it. For example, if it is related to *mpi*, double check the possible broken links of mpi during configuration or verify the working condition of the pre-installed *mpi* on the cluster. In addition, the installed programs should be compatible with the version of `Nektar++`. The full list of the compatible versions of program can be found in the [user-guide](https://www.nektar.info/getting-started/documentation/).

If some testing cases fail during *ctest*, check the log files in the **$HOME/nektar-v5.5.0/build/Testing/Temporary/** folder and identify the origin of the error.

## Execution
If Nektar++ has been compiled from source, it is convenient to add the location of the binary files to the system PATH:
```bash
export PATH="$PATH:/path/to/nektar++/build/dist/bin"
```

In the provided examples, the mesh is prepared using [gmsh](https://gmsh.info/). However, the Nektar++ meshes are also provided.

To modify the mesh, Gmsh is required. It can be installed by using binary packages provided the Linux distribution or compiling from source by running

```bash
cd $HOME
wget https://gmsh.info/bin/Linux/gmsh-2.16.0-Linux64.tgz
tar -xvzf gmsh-2.16.0-Linux64.tgz
cd gmsh-2.16.0-Linux
echo 'PATH=$HOME/gmsh-2.16.0-Linux/bin:$PATH' >> $HOME/.bashrc
source $HOME/.bashrc
```

To run the examples provided in the [example](example) folder, access the folder containing a particular example and execute the following command:

1. **convert mesh**

```bash
gmsh -2 -order 1 domain.geo
NekMesh domain.msh domain.xml
```

2. **execute simulation**

```bash
mpirun -np 4 DiffusionSolver domain.xml conditions.xml
```

3. **post-processing**

```bash
FieldConvert domain.xml domain.fld domain.vtu
```
The output file format is in *.vtu* (VTK file) which can be visualised in Paraview.

## Parameters
The key parameters to set up the simulation are listed below:

<div align="center">

| Parameter | Description |
| :---      | :---        |
| theta | angle of magnetic field  |
| B | magnitude of magnetic field |
| k_par | thermal conductivity parallel to magnetic field |
| k_perp | thermal conductivity perpendicular to magnetic field |
| epsilon | diffusion coefficient |
| A = m_i/m_p | ratio between masses of ions and proton |
| Z | ion charge state |
| lambda | Coulomb logarithm |
| TimeStep | time step size |
| NumSteps | total number of time steps |
| IO_CheckSteps | output frequency |

</div>

Typically the angle and magnitude of the magnetic field should be specified a prior. The angle is measured with respect to the positive $`x`$ axis in the clock-wise direction. Particularly, the magnitude of the magnetic field impose influences on the thermal diffusivity perpendicular to the magnetic field line. In an unsteady simulation, The values of TimeStep and NumSteps respectively are the time step size in numerical integration and the total number of the time steps. The value of IO_CheckSteps is used to control the ouptut frequency with respect to the time steps.



## References

<a name="cantwell-et-al-2015"></a>\[1\] Cantwell et al, *Nektar++: An open-source spectral/hp element framework.*, JCP, 2015 [[DOI](https://doi.org/10.1016/j.cpc.2015.02.008)]

## License

See the [LICENSE](LICENSE.txt) file for license rights and limitations (MIT).

