# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.32
# 

Name:       xapian-core

# >> macros
# << macros

Summary:    The Xapian Search Engine Library
Version:    1.4.22
Release:    1
Group:      Applications/Databases
License:    GPLv2+
URL:        https://xapian.org/
Source0:    xapian-core-%{version}.tar.xz
Source100:  xapian-core.yaml
BuildRequires:  pkgconfig(zlib)

%description
%{summary}.

Xapian is a highly adaptable toolkit which allows developers to easily
add advanced indexing and search facilities to their own applications.
It has built-in support for several families of weighting models and
also supports a rich set of boolean query operators.

%if "%{?vendor}" == "chum"
PackagedBy: nephros
Categories:
  - System
  - Utility
Links:
  Homepage: %{url}
  Help: https://www.lesbonscomptes.com/recoll/pages/documentation.html
  Donation: https://www.lesbonscomptes.com/donations/index.html
%endif


%package libs
Summary:    %{summary}
Group:      Development/Libraries
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description libs
%{summary}.

Xapian is a highly adaptable toolkit which allows developers to easily
add advanced indexing and search facilities to their own applications.
It has built-in support for several families of weighting models and
also supports a rich set of boolean query operators.

%if "%{?vendor}" == "chum"
PackagedBy: nephros
Categories:
  - Library
Links:
  Homepage: %{url}
  Help: https://www.lesbonscomptes.com/recoll/pages/documentation.html
  Donation: https://www.lesbonscomptes.com/donations/index.html
%endif


%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name}-libs

%description devel
%{summary}.

%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
# << build pre

%configure --disable-static
make %{?_smp_mflags}

# >> build post
# << build post

%install
rm -rf %{buildroot}
# >> install pre
# << install pre
%make_install

# >> install post
rm -rf %{buildroot}%{_mandir}/*
rm -rf %{buildroot}%{_docdir}/*
# << install post

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_bindir}/*
%exclude %{_bindir}/xapian-config
%dir %{_datadir}/xapian-core
%{_datadir}/xapian-core/stopwords/*.list
# >> files
# << files

%files libs
%defattr(-,root,root,-)
%{_libdir}/*.so.*
# >> files libs
# << files libs

%files devel
%defattr(-,root,root,-)
%{_bindir}/xapian-config
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
%{_libdir}/cmake/xapian/*.cmake
%{_datadir}/aclocal/xapian.m4
# >> files devel
# << files devel
