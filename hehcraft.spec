Name:           hehcraft
Version:        0.1
Release:        1%{?dist}
Summary:        Hehcraft game - a Minecraft clone with PBR shaders

License:        Apache 2
URL:            https://higui.org
Source0:        hehcraft-0.1.tar.gz

BuildRequires:  cmake, gcc, gcc-c++, glfw-devel, vulkan-loader-devel, chrpath
Requires:       glfw, vulkan-loader, mesa-libGL, libX11, libXxf86vm, libXrandr, libXi, libstdc++, glibc

%description
Hehcraft is a Minecraft clone with PBR shaders.

%global debug_package %{nil}

%prep
%setup -q -n hehcraft

%build
rm -rf build
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

%install
mkdir -p %{buildroot}/usr/local/bin
cp build/hehcraft %{buildroot}/usr/local/bin

# Очищення RPATH
chrpath --delete %{buildroot}/usr/local/bin/hehcraft || :

%files
/usr/local/bin/hehcraft

%changelog
* Tue Jul 23 2024 setbe <setbe@lafk.eu> - 0.1-1
- Initial package