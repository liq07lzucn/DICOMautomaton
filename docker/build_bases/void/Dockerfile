
FROM voidlinux/voidlinux-musl

LABEL maintainer="http://www.halclark.ca" \
      description="DICOMautomaton Void Linux build base."


WORKDIR /scratch_base
ADD docker/build_bases/void /scratch_base


# Install build dependencies.
RUN xbps-install -y -Su && \
    xbps-install -y -S \
      base-devel \
      bash \
      git \
      cmake \
      vim \
      gdb \
      rsync \
      wget \
      patchelf

#xbps-install -S -y nlopt nlopt-devel cgal cgal-devel htmlcxx htmlcxx-devel

RUN xbps-install -y -S \
    ` # Ygor dependencies ` \
        htmlcxx-devel \
        boost-devel \
        gsl-devel \
        eigen \
    ` # DICOMautomaton dependencies ` \
        eigen \
        boost-devel \
        zlib-devel SFML-devel jansson-devel \
        libpqxx-devel postgresql-client \
        cgal-devel \
        nlopt-devel \
        asio \
        freefont-ttf \
        gnuplot zenity  \
    ` # Additional dependencies for headless OpenGL rendering with SFML ` \
        xorg-minimal glu-devel \
        xorg-video-drivers xf86-video-dummy xorg-apps

#        xorg-fonts \
#        libfreeglut-devel libXi-devel libXmu-devel \
#        libboost-filesystem-dev libboost-iostreams-dev libboost-program-options-dev libboost-thread-dev \

RUN cp /scratch_base/xpra-xorg.conf /etc/X11/xorg.conf


# Install Wt from source to get around outdated and buggy Debian package.
#
# Note: Add additional dependencies if functionality is needed -- this is a basic install.
#
# Note: Could also install build-deps for the distribution packages, but the dependencies are not
#       guaranteed to be stable (e.g., major version bumps).
WORKDIR /wt
RUN git clone https://github.com/emweb/wt.git . && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr ../ && \
    make -j$(nproc) && \
    make install && \
    make clean


# Install Ygor.
#
# Option 1: install a binary package.
# ...
#
# Option 2: clone the latest upstream commit.
WORKDIR /ygor
RUN git clone https://github.com/hdclark/Ygor . && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr ../ && \
    make -j$(nproc) && \
    make install && \
    make clean


# Install Explicator.
#
# Option 1: install a binary package.
# ...
#
# Option 2: clone the latest upstream commit.
WORKDIR /explicator
RUN git clone https://github.com/hdclark/explicator . && \
    mkdir -p build && cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr ../ && \
    make -j$(nproc) && \
    make install && \
    make clean


# Install YgorClustering.
WORKDIR /ygorcluster
RUN git clone https://github.com/hdclark/YgorClustering . && \
    ./install_directly.sh

