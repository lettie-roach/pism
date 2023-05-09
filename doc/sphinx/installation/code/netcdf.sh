#!/bin/bash

set -u
set -e
set -x

# Install parallel NetCDF using parallel HDF5 in ~/local/hdf5 and
# ~/local/build/netcdf as a build directory.

hdf5=~/local/hdf5

version=4.7.4
prefix=$HOME/local/netcdf
build_dir=~/local/build/netcdf
url=https://github.com/Unidata/netcdf-c/archive/refs/tags/v${version}.tar.gz

mkdir -p ${build_dir}
pushd ${build_dir}

wget -nc ${url}
tar zxf v${version}.tar.gz

pushd netcdf-c-${version}

export CFLAGS="-g -O0"
unset CFLAGS

CC=mpicc CPPFLAGS=-I${hdf5}/include LDFLAGS=-L${hdf5}/lib ./configure \
        --enable-netcdf4 \
        --disable-dap \
        --prefix=${prefix} 2>&1 | tee netcdf_configure.log

make all 2>&1 | tee netcdf_compile.log
make install 2>&1 | tee netcdf_install.log

popd
popd
