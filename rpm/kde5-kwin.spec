# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.22
# 
# >> macros
# << macros

Name:       kde5-kwin
Summary:    KDE Window manager
Version:    4.97.0
Release:    1
Group:      System/Base
License:    GPLv2+
URL:        http://www.kde.org
Source0:    %{name}-%{version}.tar.xz
Source100:  kde5-kwin.yaml
Source101:  kde5-kwin-rpmlintrc
Patch0:     icccm-optional.diff
Requires:   kde5-filesystem
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Xml)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Concurrent)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(Qt5Script)
BuildRequires:  pkgconfig(Qt5X11Extras)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5UiTools)
BuildRequires:  kde5-rpm-macros
BuildRequires:  extra-cmake-modules
BuildRequires:  qt5-tools
BuildRequires:  kf5-kconfig-devel
BuildRequires:  kf5-kconfigwidgets-devel
BuildRequires:  kf5-kcoreaddons-devel
BuildRequires:  kf5-kcrash-devel
BuildRequires:  kf5-kglobalaccel-devel
BuildRequires:  kf5-ki18n-devel
BuildRequires:  kf5-kinit-devel
BuildRequires:  kf5-knotifications-devel
BuildRequires:  kf5-kservice-devel
BuildRequires:  kf5-plasma-devel
BuildRequires:  kf5-kwidgetsaddons-devel
BuildRequires:  kf5-kwindowsystem-devel
BuildRequires:  kf5-kdoctools-devel
BuildRequires:  kf5-kcmutils-devel
BuildRequires:  kf5-knewstuff-devel
BuildRequires:  kf5-kactivities-libs-devel
BuildRequires:  kf5-kdoctools-devel
BuildRequires:  libGL-devel
BuildRequires:  libEGL-devel
BuildRequires:  libxkbcommon-devel
BuildRequires:  libX11-devel
BuildRequires:  libxcb-devel
BuildRequires:  libICE-devel
BuildRequires:  libSM-devel
BuildRequires:  libXcursor-devel
BuildRequires:  xcb-util-devel


%description
KDE Window manager



%package devel
Summary:    Development files for %{name}
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   kf5-kconfig-devel
Requires:   kf5-kwidgetsaddons-devel

%description devel
The %{name}-devel package contains the files necessary to develop applications
that use %{name}.


%package doc
Summary:    User manual for %{name} %{name}
Group:      Documentation
Requires:   %{name} = %{version}-%{release}

%description doc
User manual for %{name}.



%prep
%setup -q -n %{name}-%{version}/upstream

# icccm-optional.diff
%patch0 -p1
# >> setup
# << setup

%build
# >> build pre
%kde5_make
# << build pre



# >> build post
# << build post
%install
rm -rf %{buildroot}
# >> install pre
%kde5_make_install
# << install pre

# >> install post
# << install post












%files
%defattr(-,root,root,-)
%doc COMPLIANCE COPYING COPYING.DOC HACKING README
%{_kde5_bindir}/kwin
%{_kde5_datadir}/kwin
%{_kde5_libdir}/libkdeinit5_kwin.so
%{_kde5_libdir}/libkdeinit5_kwin_rules_dialog.so
%{_kde5_libdir}/libkdecorations.so.*
%{_kde5_libdir}/libkwinxrenderutils.so.*
%{_kde5_libdir}/libkwineffects.so.*
%{_kde5_libdir}/libkwinglesutils.so.*
%{_kde5_libdir}/libkwin4_effect_builtins.so.*
%{_kde5_libdir}/plugins/*.so
%{_kde5_libdir}/plugins/kwin
%{_kde5_libdir}/qml/org/kde/kwin
%{_kde5_libdir}/kconf_update_bin/kwin5_update_default_rules
%{_kde5_libexecdir}/kwin_killer_helper
%{_kde5_libexecdir}/kwin_rules_dialog
%{_kde5_datadir}/kwincompositing
%{_datadir}/kservices5/*.desktop
%{_datadir}/kservices5/kwin
%{_datadir}/kservicetypes5/*.desktop
%{_datadir}/knotifications5/kwin.notifyrc
%{_datadir}/config.kcfg/kwin.kcfg
%{_datadir}/icons/oxygen/*
%{_kde5_sysconfdir}/xdg/*.knsrc
# >> files
# << files


%files devel
%defattr(-,root,root,-)
%{_libdir}/cmake/KWinDBusInterface
%{_libdir}/cmake/KDecorations
%{_datadir}/dbus-1/interfaces/*.xml
%{_kde5_libdir}/libkdecorations.so
%{_kde5_libdir}/libkwinxrenderutils.so
%{_kde5_libdir}/libkwineffects.so
%{_kde5_libdir}/libkwinglesutils.so
%{_kde5_libdir}/libkwin4_effect_builtins.so
%{_kde5_includedir}/*.h
# >> files devel
# << files devel

%files doc
%defattr(-,root,root,-)
%doc COMPLIANCE COPYING COPYING.DOC HACKING README
%{_datadir}/doc/HTML/en/kcontrol/*
# >> files doc
# << files doc

