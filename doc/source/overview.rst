Overview
========

The concept of a wordclock is widely known, and I think it does not need further introduction: It reads the time to you. 

I am also not the first person to turn this clock into a DIY project. A quick web search for the terms "DIY" and "wordclock" yields plenty or results. My realisation is also not the most advanced. Nor the simplest. Or even the fastest to realize. So why did I construct my own, instead of using one of the plenty models around the interwebs?

Well, first of all, I like to tinker and fancied the idea to do it all on my own. I wanted to start on a clean slate. Had I known how much time one can spend when starting with a clean canvas, I'd probably better have chosen an existing realization. 

Let me briefly describe my design paradigms before digging into the construction and how this clock works. If you are still following then, I will point you to further material in case you wish to realize your own OpenWordClock. 

Design paradigms
----------------
From the very start of this project, I tried to follow a few but significant design paradigms for everything related to this project. In no particular order, these are. 

1. OpenWordClock should as closely as possible resemble the appearance of the original `Qlocktwo <https://qlocktwo.com/>`_.

2. All parts of the project must be realizable as a hobbyist with access to a public makerspace or similar. 

3. OpenWordClock was always intended to be published free and open source. 

Of course, I had to adapt the face of OpenWordClock to match commercially available components. Also, the choice of microcontroller limits the number of features to a subset of the original. 

To be able to realize all this as a hobbyist, I resorted to using free and open source software, whenever possible. The 3D models and technical drawoings are realized in `FreeCAD <https://www.freecad.org/>`_, further sketches are done with `Inkscape <https://inkscape.org/>`_. To design the logic board, I used `KiCAD <https://www.kicad.org/>`_ and for source code development I used the `Arduino IDE <https://www.arduino.cc/en/software>`_ and its libraries. 

For the actual build, I consulted my local makerspace at the `Dynamo <https://www.dynamo.ch/>`_ in Zurich, where I had access to laser cutting, 3D-printing and foil plotting equipment.

Finally, published as a free an open source project, everybody is invited to realize their own OpenWordClock. And of course everybody is invited to adapt it to their personal needs.


Construction
------------


How does it work?
-----------------


Where to start?
---------------
