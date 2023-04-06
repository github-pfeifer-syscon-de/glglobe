A nice desktop clock in earth shape.

Additional formats:
%D earth declination (for northern hemisphere)
%rise sun rise for the location setup by latitude / longitude
%set sun set
\n new line
%weather for the used weather time

![Glglobe](glglobe.png "glglobe")

configure:
     allow --with-gles using GL ES 3 e.g. useful on Raspi's (requires same use on GenericGlm)

Uses: https://re-d.ssec.wisc.edu/ for weather display
(some better mercator unmapping was implemented)

Added option to display Geo.json files (presume EPSG:4326 coordinates (based on WGS 84 at the used scale the differences should not matter)).
As there are many files with a high density of points around there are some limits implemented, just in case your might wonder.
The function uses the shortest path between the given points,
but as we live on a sphere (almost) that might hide some part of the lines (these cases should be rare,
and i didn't try to bend the lines as i live in some aged country with "wrinkles" all over).

Build with autotools, requires genericGlm (see there for some basic build infos).
The files are not very portable, if you dont use gnu tools there might be issues.

In directory res there are some images required i didn't add here for the possible licence issues.
They are download when build with curl, so if you are sensitve to licence issue, or want to use a different download-tool see res/Makefile.am.
