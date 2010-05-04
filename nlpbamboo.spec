%define aprver 0

Summary: A Chinese Segmentation Library
Name: nlpbamboo
Version: 1.1.1
Release: 2
License: BSD
Group: Development/Libraries
URL: http://code.google.com/p/nlpbamboo/
Source0: %{name}-%{version}.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildPrereq: CRF++ 

%description
A Chinese Segmentation Library

%package devel
Summary: A Chinese Segmentation Library 
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
A Chinese Segmentation Library

%prep
%setup -q

%build
# regenerate configure script etc.
./configure
#cmake . -DCMAKE_BUILD_TYPE=release -DLIB_INSTALL_DIR=%{_libdir}
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libbamboo.so
%{_libdir}/libbamboo.so.*
%{_libdir}/libbamboo.a
/opt/bamboo/*

%files devel
%defattr(-,root,root,-)
%{_includedir}/bamboo/*

%changelog
* Thu Feb 19 2009 Jianing Yang <detrox@gmail.com> 1.1.1
- initial build
