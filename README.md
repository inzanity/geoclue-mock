# geoclue-mock a location faker for GeoClue

This little location provider for geoclue can be used to fake location.
It provides location set in gsettings to GeoClue.

## Installation

```
autoreconf -i
./configure
make install
glib-compile-schemas /usr/share/glib-2.0/schemas
```

## Usage

```
gsettings set fi.inz.GeoclueMock latitude 12.34
gsettings set fi.inz.GeoclueMock longitude 43.21
gsettings set fi.inz.GeoclueMock fuzz 0.01
gesttings set fi.inz.GeoclueMock active true
```

Then launch your favourite app that uses geoclue and it should get the faked position.
