# Glglobe

A nice desktop clock in earth shape, with
- moon-phase
- with worldtime
- satelite images
- geo.json display

![Glglobe](glglobe.png "glglobe")

## Prerequisits

see genericImg, genericGlm, geodata

## Build

The directory res requires some images, i didn't add here for the possible licence issues.
They are download when build with curl, so if you are sensitve to licence issue, or want to use a different download-tool see res/Makefile.am.

configure:
     allow --with-gles using GL ES 3 e.g. useful on Raspi's (requires same use on GenericGlm)


Build for windows was integrated at least for msys2<br>
<pre>
  look for "$host_os" = "mingw32"
</pre>

## Costomizing

Additional formats:
%D earth declination (for northern hemisphere)<br>
%rise sun rise for the location setup by latitude / longitude<br>
%set sun set<br>
\n new line<br>
%weather for the used weather time<br>


Added option to display Geo.json files (presume EPSG:4326 coordinates (based on WGS 84 at the used scale the differences should not matter)).
As there are many files with a high density of points around there are some limits implemented, just in case your might wonder.
The function uses the shortest path between the given points,
but as we live on a sphere (almost) that might hide some part of the lines (these cases should be rare,
and i didn't try to bend the lines as i live in some aged country with "wrinkles" all over).


## Navigation

The usual navigation options are a bit reduced here.
Only the earth can be "moved" by:

Mouse scroll -> change distance

Drag right mouse buttton -> roll

## Config-file

For some tweaking the config-file is helpful:
<pre>glglobe.conf</pre>
for linux it is found in user-home/.config,
for windows user-home/appData/Local.

## Configuration

The most obvious option use the dialog.
For some advanced options use the config-file (see above).

## Troubleshooting

First check the application log, the location depends on genericImg --with-... switches.
The default location is the log directory within user-home.
But some messages may still get spilled to the console.
If more infos are required change config-file (see above)
<pre>
[globe]
logLevel=
</pre>
 to one of Severe, Alert, Crit, Error, Warn, Notice, Info, Debug (and restart).

For advanced issues there are some defines e.g. CONFIG_DEBUG that might help in identifying more specific issues.
