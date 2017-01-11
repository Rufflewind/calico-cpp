# Maintainer: Phil Ruffwind <rf@rufflewind.com>
pkgname=calico-git
pkgver=0.9.0.r32.16b7a07
pkgrel=1
pkgdesc="Utilities for C++"
arch=(any)
url=https://github.com/Rufflewind/calico-cpp
license=(MIT)
makedepends=(git)
source=($pkgname::git://github.com/Rufflewind/calico-cpp)
sha256sums=(SKIP)

pkgver() {
    cd "$srcdir/$pkgname"
    s=`git 2>/dev/null describe --long --tags`
    if [ $? -eq 0 ]
    then
        printf '%s' "$s" | sed 's/^v//;s/\([^-]*-\)g/r\1/;s/-/./g'
    else
        printf 'r%s.%s' "`git rev-list --count HEAD`" \
               "`git rev-parse --short HEAD`"
    fi
}

build() {
    cd "$srcdir/$pkgname"
    make
}

package() {
    cd "$srcdir/$pkgname"
    make DESTDIR="$pkgdir" PREFIX=/usr install
    install -Dm644 LICENSE.md "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
