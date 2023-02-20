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

To be able to realize all this as a hobbyist, I resorted to using free and open source software, whenever possible. The 3D models and technical drawings are realized in `FreeCAD <https://www.freecad.org/>`_, further sketches are done with `Inkscape <https://inkscape.org/>`_. To design the logic board, I used `KiCAD <https://www.kicad.org/>`_ and for source code development I used the `Arduino IDE <https://www.arduino.cc/en/software>`_ and its libraries. 

For the actual build, I consulted my local makerspace at the `Dynamo <https://www.dynamo.ch/>`_ in Zurich, where I had access to laser cutting, 3D-printing and foil plotting equipment.

Finally, published as a free an open source project, everybody is invited to realize their own OpenWordClock. And of course everybody is invited to adapt it to their personal needs.


Construction
------------

.. figure:: ./img/openwordclock_02.jpg
   :width: 70 %
   :align: center
   :alt: Image of the construction of OpenWordClock

   Internal construction of OpenWordClock. The slotted structure of the separator grid and the corner blocks are clearly visible. 

The body is constructed from slotted MDF parts and a matching baseplate. All is held in place with screws and some glue on 4 corner blocks with heat set threaded inserts. For a nice outer finish, the frame is wrapped with edging strips.

The outer measurement with 450 mm x 450 mm match the measurements of the original. The body is 25 mm deep (22 mm internally). The slotted structure forms a separator grid matching the pitch of commercially available LED strips with 30 LEDs per meter. The result is a square 330 mm x 330 mm grid with 11 columns of 10 LEDs. Each column contains a strip with 10 LEDs with one more LED in every corner.

The baseplate holds all electronics, apart from controls, that are placed in the outer frame. The 3 control buttons and the power switch can be either placed on the left or on the right. Similarly, the logic board can be mounted on the left or on the right. The 5.5 mm / 2.5 mm barrel jack for power is always on the bottom. A photodiode for room brightness detection is placed in the top. 

.. figure:: ./img/openwordclock_01.jpg
   :width: 70 %
   :align: center
   :alt: OpenWordClock with front panel. 

   OpenWordClock with laser cut stainless steel front panel. A polystyrene diffusor evens the illumination of the letters.

I tested two different styles of front panels: laser cut stencils or foil masks on diffusor plates. Both styles yield excellent results and are suitable for different finishes. 

The laser cut stencil approach is best for wood or metal front panels, where one would like to preserve the surface structure of the original material. The letters are cut from the material stock, then a diffusor is applied behind all cutouts. 

The foil approach uses a foil as mask, that has the advantage, that it can be custom printed unlike complex material surfaces. After printing and cutting on a foil plotter, the cutout letters are removed and the foil is transferred to an acrylic or polystyrene sheet with transfer foil. 

In either case, the front panel is held on the clock with 8 magnets with matching counterparts in the corner blocks. The magnets in the corner blocks are countersunk to yield a tight fit and prevent sliding of the front panel. 



How does it work?
-----------------


Where to start?
---------------
