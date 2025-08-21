TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += ImgCompressLib
SUBDIRS += ImgCompressUI

ImgCompressUI.depends = ImgCompressLib
