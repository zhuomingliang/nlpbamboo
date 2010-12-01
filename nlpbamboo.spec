%define aprver 0

Summary: A Chinese Natural Language Processing System
Name: nlpbamboo
Version: 1.1.2
Release: 1
License: BSD
Group: Development/Libraries
URL: http://code.google.com/p/nlpbamboo/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-buildroot
BuildPrereq: CRF++ 

%description
NLPBamboo(bamboo for short) is an open source Chinese language processing
system. It focuses on "Chinese Word Segmentation", "Topic Word Extraction" 
and "Name Entity Recognization". 

%package devel
Summary: A Chinese Segmentation Library 
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Developmental libraries and header files required for developing or
compiling software which links to the nlpbamboo library, which is 
an open source Chinese natural language processing system.

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

%changelog
* Wed Dec  1 2010 Jianing Yang <jianingy.yang AT gmail DOT com> 1.1.1
- Updated description text. Modified build & setup process for Dr.Xu's 
  automake build system

* Thu Feb 19 2009 Jianing Yang <jianingy.yang AT gmail DOT com> 1.1.1
- initial build
