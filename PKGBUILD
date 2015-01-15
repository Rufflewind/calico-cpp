# Maintainer: Phil Ruffwind <rf@rufflewind.com>
pkgname=calico-git
pkgver=latest
pkgrel=1
pkgdesc="Utilities for C++"
arch=(any)
url=https://github.com/Rufflewind/calico
license=(MIT)
makedepends=(git)
source=($pkgname::git://github.com/Rufflewind/calico)
sha256sums=(SKIP)

pkgver() {
    cd "$srcdir/$pkgname"
    git rev-parse --short HEAD
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
