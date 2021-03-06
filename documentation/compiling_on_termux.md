
# Overview

`DICOMautomaton` can be compiled on `termux` for use on `Android`. Note that there is no `DICOMautomaton` package for
`termux`. The following instructions can be used to install `DICOMautomaton` by compiling directly on the device. 

Note that few dependencies are currently available out-of-the-box as `termux` packages, so `DICOMautomaton`
functionality is limited unless the enabling dependencies are manually installed. In particular, no graphical facilities
are available (as of January 2020).

Compilation times will strongly depend on the device. On a modest 2-core `armv7l` `Motorola E4` it will take around 10
hours and require 1 GB of RAM without using parallel builds.

# `Termux` preparation

Install `termux`. Then install the base development meta-package:

    $>  pkg install build-essential

# Packaged External Dependencies

Install all required dependencies:

    $>  pkg install boost cmake eigen ncurses gsl

Install `asio`, which is header-only:

    $>  ( cd ~/ &&
          mkdir -p ~/dcma &&
          cd ~/dcma &&
          wget 'https://sourceforge.net/projects/asio/files/latest/download' -O asio.tgz &&
          tar -axf asio.tgz &&
          cd asio-*/ &&
          cp -v -R include/asio/ include/asio.hpp $PREFIX/include/ )

## `Explicator`

Download and install via:

    $>  ( cd ~/ &&
          mkdir -p ~/dcma &&
          cd ~/dcma &&
          git clone https://github.com/hdclark/explicator explicator &&
          cd explicator &&
          mkdir build &&
          cd build &&
          cmake -E env CXXFLAGS='-march=native' \
          cmake \
            -DCMAKE_INSTALL_PREFIX="$PREFIX" \
            -DCMAKE_BUILD_TYPE=Release \
            .. && \
          make VERBOSE=1 &&
          make install )

Note the use of the `termux`-specific installation prefix `PREFIX`. Super-user privileges are **not** required since the
system prefix directory is not protected.

## `YgorClustering`

Download and install via:

    $>  ( cd ~/ &&
          mkdir -p ~/dcma &&
          cd ~/dcma &&
          git clone https://github.com/hdclark/ygorclustering ygorclustering &&
          cd ygorclustering &&
          install -v -Dm644 src/*hpp "$PREFIX"/include/ )

## `Ygor`

Download and install via:

    $>  ( cd ~/ &&
          mkdir -p ~/dcma &&
          cd ~/dcma &&
          git clone https://github.com/hdclark/ygor ygor &&
          cd ygor &&
          mkdir build &&
          cd build &&
          cmake -E env CXXFLAGS='-march=native' \
          cmake \
            -DCMAKE_INSTALL_PREFIX="$PREFIX" \
            -DCMAKE_BUILD_TYPE=Release \
            -DWITH_LINUX_SYS=ON \
            -DWITH_GNU_GSL=ON \
            -DWITH_EIGEN=ON \
            .. && \
          make VERBOSE=1 &&
          make install )


# `DICOMautomaton`

Download and install via:

    $>  ( cd ~/ &&
          mkdir -p ~/dcma &&
          cd ~/dcma &&
          git clone https://github.com/hdclark/dicomautomaton dicomautomaton &&
          cd dicomautomaton &&
          `#  Ignore bash completions by nullifying the installation script. ` \
          echo " " > scripts/CMakeLists.txt &&
          mkdir build &&
          cd build &&
          cmake -E env CXXFLAGS='-march=native' LDFLAGS='-liconv' \
          cmake \
            -DCMAKE_INSTALL_PREFIX="$PREFIX" \
            -DCMAKE_BUILD_TYPE=Release \
            -DMEMORY_CONSTRAINED_BUILD=OFF \
            -DWITH_CGAL=OFF \
            -DWITH_WT=OFF \
            -DWITH_GNU_GSL=OFF \
            -DWITH_SFML=OFF \
            -DWITH_NLOPT=OFF \
            -DWITH_POSTGRES=OFF \
            -DWITH_JANSSON=OFF \
            .. && \
          make VERBOSE=1 &&
          make install )


# Portable Binaries

Portable binaries require a modification to the relevant script:

    rsync -L -r --delete \
      $( ldd "${which_dcma}" | 
    -     grep '=>' | 
    -     sed -e 's@.*=> @@' -e 's@ (.*@@' 
    -  ) \
    + sed -e "s@^@$PREFIX/lib/@g" ) \
       "${out_dir}/"


# Conclusion

Confirm that the installation works by issuing:

    $>  dicomautomaton_dispatcher -h

Note that the installation will generally **not** be portable due to use of `-march=native`.

Also note that Termux provides static library versions of many of `DICOMautomaton`s dependencies.
Their use may help to improve portability.

