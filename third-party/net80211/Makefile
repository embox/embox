# net80211 (NetBSD wireless stack + urtwn) fetched from its own repository.
#
# PKG_SOURCES is a git URL: extbld clones the repository's default branch
# (main) and copies the checkout into each module build dir, where embox
# compiles the sources directly. Bump by re-running make after the remote
# main advances (or pin PKG_VER to a commit for a reproducible build).
#
# Checkout layout:
#   net80211/     NetBSD 802.11 stack (verbatim import)
#   driver/urtwn/ RTL8188EU driver
#   compat/netbsd/ shadow headers for the NetBSD kernel API
#   port/         port interface + embox shims + cherryusb backend

PKG_NAME := net80211
PKG_SOURCES := https://github.com/KevinACoder/net_80211.git
