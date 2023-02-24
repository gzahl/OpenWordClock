.. _sub-parts:

Parts List
----------

.. warning::
   The following parts lists may contain errors and are published without any warranty. Please carefully review before ordering components.


Body parts and fasteners
~~~~~~~~~~~~~~~~~~~~~~~~

The table below lists all parts necessary for the assembly of the body of an OpenWordClock. All laser cut parts are cut from a single 600 mm x 900 mm sheet of 3 mm thick MDF using `this template <https://gitlab.com/JacobNuernberg/openwordclock/-/blob/main/cad/Wordclock_LaserTemplate_900x600_optimized.pdf>`_. For a nice surface finish, I recommend using MDF with a white surface coating. The stock material as well as the edge band can be sourced in your local hardware store. 

The corner blocks are 3D printed in PLA with 5-15 % infill and can be printed with rather thick layers (0.15 mm). As these are internal parts, they are not seen in the final outcome.

.. list-table:: Body Parts
   :widths: auto 
   :header-rows: 1
   :align: center

   * - Qty. 
     - Description
     - Material
     - Comment
   * - 1
     - Baseplate
     - MDF (3mm)
     - Laser cut
   * - 2
     - Vertical long
     - MDF (3mm)
     - Laser cut
   * - 2
     - Horizontal long
     - MDF (3mm)
     - Laser cut
   * - 10
     - Vertical short
     - MDF (3mm)
     - Laser cut
   * - 9
     - horizontal long
     - MDF (3mm)
     - Laser cut
   * - 1
     - Top
     - MDF (3mm)
     - Laser cut
   * - 1
     - Bottom
     - MDF (3mm)
     - Laser cut
   * - 1
     - Left
     - MDF (3mm)
     - Laser cut
   * - 1
     - Right
     - MDF (3mm)
     - Laser cut
   * - 4
     - Corner block 
     - PLA
     - 3D print
   * - 1
     - Edge band > 25 mm wide
     - 
     - approx. 2 m

The body is held in place by a bunch of fasteners listed in the table below. The table also lists fasteners needed to attach the control board and the front panel. These should also be available in your local hardware store. 

.. list-table:: Fasteners
   :widths: auto
   :header-rows: 1
   :align: center

   * - Qty. 
     - Description
   * - 16
     - M3 heat set insert
   * - 16
     - M3x6 Button head torx screw
   * - 16
     - M3 Washer
   * - 4 
     - M2x6 standoff
   * - 8
     - M2x5 Button head torx screw
   * - 8 
     - M2 washer
   * - 16
     - Magnet round 5 mm x 2 mm

For the front panel, the required materials and parts depend on the preferred style of front panel. The table lists the parts for the two styles of front panel, that I have worked with. 

.. list-table:: Front panel components
   :widths: auto
   :header-rows: 1
   :align: center

   * - Qty. 
     - Description
     - Dimensions
   * - 1 
     - Polystyrene sheet (3mm)
     - 450 mm x 450 mm
   * - 1
     - Foil stencil
     - ca. 500 mm x 500 mm
   * - **or**
     - 
     - 
   * - 1
     - Laser cut stencil
       in material of choice
     - 450 mm  x 450 mm
   * - 1
     - Polystyrene diffusor (0.5 mm)
     - approx. 0.2 m\ :sup:`2`


Control board and electronics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The control board of the OpenWordClock can be realized in two ways: Either you take an off-the-shelf Arduino Nano and pair it with a DS3231 RTC evaluation board or you decide to order and assemble the custom control pcb according to `the schematics provided in the repository <https://gitlab.com/JacobNuernberg/openwordclock/-/tree/main/pcb>`_. For the custom PCB, consult your preferred PCB producer; usually it is possible to order a small quantity of boards. The table below lists the components needed for the latter case. The part numbers refer to the part on `Digi-Key <https://digikey.com>`_, where I sourced all components.

.. note::
   The tables list the quantities needed for a single control board. However, ordering in bulk may significantly reduce the price for the individual board. That is true for the board on the one hand side, and for the components on the other side as well. 

   From my experience, ordering 10, instead of 1 board, may cut the price per board in half.

.. list-table:: Custom control pcb comonents
   :widths: auto 
   :header-rows: 1

   * - Qty.
     - Description
     - Value
     - Digi-Key part No. 
   * - 1 
     - Custom PCB
     - 
     - 
   * - 1
     - Battery holder BS-7
     - 
     - BS-7-ND
   * - 1
     - Battery CR2032
     - 
     - 2984-CR2032-H-ND
   * - 3
     - Capacitor
     - 1 uF
     - 445-FK24X7R1H105KR000-ND
   * - 5
     - Capacitor
     - 100 nF
     - 399-9877-1-ND
   * - 2
     - Diode 1N5818
     - 
     - 2368-1N5818-ND
   * - 1
     - Micro-USB connector
     - 
     - WM1714CT-ND
   * - 1
     - Pin header 90° 1x3
     - 
     - WM19494-ND
   * - 1 
     - Pin header 90° 1x8
     - 
     - WM4106-ND
   * - 1
     - Pin header 2x3
     - 
     - WM23646-ND
   * - 1 
     - Pin header 2x4
     - 
     - WM8140-ND
   * - 1
     - LED 
     - Green
     - 732-5017-ND
   * - 2
     - LED 
     - Yellow
     - 732-5018-ND
   * - 1
     - LED 
     - Red
     - 732-5016-ND
   * - 4 
     - Resistor
     - 10 k
     - 10.0KXBK-ND
   * - 4 
     - Resistor
     - 1 k
     - 1.00KXBK-ND
   * - 4 
     - Resistor
     - 4.7 k
     - 4.7KQBK-ND
   * - 4 
     - Resistor
     - 1 M
     - 1.0MQBK-ND
   * - 1
     - Thermal resettable fuse MFFSMF050
     - 
     - MF-FSMF050X-2
   * - 1
     - Miniature push button
     - 
     - 450-1650-ND
   * - 1
     - DIP switch 3x
     - 
     - 208-3
   * - 1
     - USB controller FT232RL
     - 
     - 768-1007
   * - 1 
     - Microcontroller ATmega328 p
     - 
     - ATMEGA328P-PU
   * - 1 
     - DIP socket 2x14
     - 
     - ED3050-5-ND
   * - 1
     - RTC DS3231MZ
     - 
     - DS3231MZ+
   * - 1 
     - Memory AT24CS32
     - 
     - AT24D32D-SSHM-B
   * - 1
     - Crystal oscillator
     - 16 MHz
     - 2151-AS-16.000-18-ND


Independent of how you realize the control board, the following electronic components are needed. These are the buttons to control the clock, the LEDs and any other components to guarantee functionality. 


.. list-table:: Other electronic components
   :widths: auto 
   :header-rows: 1

   * - Qty. 
     - Description
     - 
     - Digi-Key part No.
   * - 1
     - 5.5/2.5 mm low voltage barrel plug
     - 
     - 
   * - 1
     - Power switch
     - 
     - 
   * - 3
     - Push button 
     - 
     - 
   * - 1
     - 5 mm LED socket
     - 
     - 
   * - 1 
     - Photodiode (5 mm LED packaging)
     - 
     - 
   * - 1 
     - LED strip WS2812B (30/m)
     - 5 m
     - 
   * - 2 
     - Wago clamp 3x
     - 
     - 
   * - 1
     - Wago clamp 2x
     - 
     - 
   * - 1
     - Wire red 0.5 mm\ :sup:`2`
     - ca. 2.5 m
     - 
   * - 1
     - Wire black 0.5 mm\ :sup:`2`
     - ca. 2.5 m
     - 
   * - 1
     - Wire blue 0.25 mm\ :sup:`2`
     - ca. 2.5 m
     - 
   * - 1
     - Signal wire various colors 0.14 mm\ :sup:`2`
     - ca. 2.5 m
     - 

Here, the red and black cable cary power to the LEDs, the thinner blue carries the signal. Various color signal wire is used to connect buttons and the photodiode. The WAGO clamps help to connect the LEDs.  
