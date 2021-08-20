<p align="center">
  <a href="https://www.nektar.info/" target="_blank" >
    <img alt="Nektar++ diffusion" src="readme/icon-blue.png" width="100" />
  </a>
</p>

<p align="center">
  <a href="https://gitlab.nektar.info/neptune/nektar-diffusion/LICENSE.txt" target="_blank">
    <img alt="Software License" src="https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat-square">
  </a>
</p>

<h1>Nektar-diffusion proxy-app</h1>

## Table of contents

  * [Description](#description)
  * [Installation and dependencies](##installation-and-dependencies)
    * [Installing Nektar++ from source](#installing-nektar++-from-source)
    * [Installing proxy-app](###installing-proxy-app)
  * [XML Session File](##xml-session-file)
  * [Execution](##execution)
  * [References](##references)
  * [License](##license)

## Description
**Nektar-diffusion proxy-app**: An anisotropic thermal conduction proxy-app for the magnetized plasma written in Nektar++ framework[(Cantwell et al 2015)](#cantwell-et-al-2015). The derivation of the anisotropic thermal conduction in the magnetized plasma and its variational formulation are documented in the [/docs/](https://gitlab.nektar.info/neptune/nektar-diffusion/-/tree/steady_b/docs) folder. For the detailed formulations and tutorials of Nektar++, please refer to the user-guide and developer-guide at the home page of [Nektar++](https://www.nektar.info/). Some examples are provided in the [example](https://gitlab.nektar.info/neptune/nektar-diffusion/-/tree/steady_b/example) folder. 

## Installation and dependencies
The compilation of Nektar-diffusion proxy-app requires a C++ compiler, `CMake` and `Nektar++`. To successfully compile and install `Nektar++`, the following programs and libraries are also necessary: `scotch`, `boost`, `flex`, `tinyxml` (for parsing parameters and setting up simulation), `blas` and `lapack` (for linear algebra). Some of these are required and must be installed prior to compiling `Nektar++`, most of which are available as pre-built system packages on most Linux distributions, installed manually by a user (typically the development packages with a *-dev* or *-devel* suffix is required) or can be downloaded and compiled by Nektar++ automatically (by specifying the corresponding flags of configuration), e.g., `tinyxml`. It is important to make sure that their binaries are searchable in the path of the system.

In addition, `Nektar++` can be linked to some optional packages in order to initiate a range of features: `openmpi` (for parallel computing), `hdf5` (for large-scale data storage and parallel input/outoput), `metis` (for domain partitioning), `PETSc` (for extra solvers) and `fftw` (for Fast Fourier Transforms). 

The typtical procedures of installation are presented below. It is recommended to use the pre-built packages, e.g., `openmpi`, in the system, especially compiling `Nektar++` on the cluster.  For the additional information on the options of configuration and the full range of different features, please refer to the user-guide of [Nektar++](https://www.nektar.info/downloads/).

### Installing Nektar++ from source
Clone the repository of `Nektar++` from [https://gitlab.nektar.info/nektar/nektar](https://gitlab.nektar.info/nektar/nektar) or download the tarball of `Nektar++` from [https://www.nektar.info/downloads/](https://www.nektar.info/downloads/) and put it into the home directory (**$HOME**). Assuming the file name of the tarball is **nektar-5.0.2.tar.gz**, execute the following commands to install `Nektar++` in the default directory ($HOME/nektar-5.0.2/build/dist):

```bash
>tar -xvzf nektar-5.0.2.tar.gz
>cd nektar-5.0.2
>mkdir build && cd build 
>cmake -DNEKTAR_USE_MPI=ON -DNEKTAR_USE_SCOTCH=ON -DNEKTAR_USE_SYSTEM_BLAS_LAPACK=ON -DNEKTAR_USE_HDF5=ON -DTHIRDPARTY_BUILD_TINYXML=ON ..
>make -j 4 install
>ctest
```
The command *ctest* is used to run a series of testing cases to verify the correct installation.   

**Curses interface:** 

Alternatively, to specify the full range of flags of configuration using the curses interface to `CMake`, the following commands can be executed to install `Nektar++`:

```bash
>tar -xvzf nektar-5.0.2.tar.gz
>cd nektar-5.0.2
>mkdir build && cd build 
>ccmake ..
>make -j 4 install
>ctest
```
By executing the command *ccmake ..*, all flags of configuration are listed and displayed. For a typical installation, the following flags, NEKTAR_USE_MPI, NEKTAR_USE_SCOTCH, NEKTAR_USE_SYSTEM_BLAS_LAPACK and NEKTAR_USE_HDF5, should be switched on by pressing *enter* key. Subsequently, pressing *c* key until the option *Press [g] to generate and exit* appears. Finally, press *g* to generate the configuration file. 

**Debugging compilation, installation and testing:**

If the compilation fails, check the possible broken links during the configuration by the command *ccmake ..* and togger it to the advanced mode by press the *t* key. A list of detailed links to the libraries will appears for investigation.

If an error was observed during installation, take note the description of this error and identify which program causes it. For example, if it is related to *mpi*, double check the possible broken links of mpi during configuration or verify the working condition of the pre-installed *mpi* on the cluster.

If some testing cases fail during *ctest*, check the log files in the **$HOME/nektar-v5.0.2/build/Testing/Temporary/** folder and identify the origin of the error.

### Installing proxy-app

## XML Session file
The Nektar++ native file format is compliant with XML version 1.0. A xml session file is used as an input file to communicate with Nektar++ libraries, e.g., store meshes, set parameters, choose solvers and define boundary conditions. A brief introduction of the xml session file is provided below.

## Execution

## References

#### (Cantwell et al 2015) 
*Nektar++: An open-source spectral/hp
element framework.* [[DOI](https://doi.org/10.1016/j.cpc.2015.02.008)]

## License

See the [LICENSE](LICENSE.txt) file for license rights and limitations (MIT).

