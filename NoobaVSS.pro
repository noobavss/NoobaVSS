
##########################################################
# Creating Visual Studio Project Files
##########################################################

# Developers using Visual Studio to write Qt applications can use the Visual
# Studio integration facilities provided with the Qt Commercial Edition and do
# not need to worry about how project dependencies are managed.

# However, some developers may need to import an existing qmake project into
# Visual Studio. qmake is able to take a project file and create a Visual Studio
# project that contains all the necessary information required by the
# development environment. This is achieved by setting the qmake project
# template to either vcapp (for application projects) or vclib
# (for library projects).

# This can also be set using a command line option, for example:

# qmake -tp vc

# It is possible to recursively generate .vcproj files in subdirectories and
# a .sln file in the main directory, by typing:

# qmake -tp vc -r

# Each time you update the project file, you need to run qmake to generate an
# updated Visual Studio project.

# Note: If you are using the Visual Studio Add-in, you can import .pro files
# via the Qt->Import from .pro file menu item.

###########################################################

TEMPLATE = subdirs

SUBDIRS += NoobaPluginAPI NoobaFE

CONFIG += ordered   # builds in the order of subdirs mentioned
