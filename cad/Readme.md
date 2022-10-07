# CAD design

The design for the clock was entirely drawn up in FreeCAD. The laser templates were then extracted from 1:1-Scale technical drawings and refined with Inkscape. 

## Laser templates
The design is an easy to assemble, slotted frame laser cut from 3 mm thick MDF sheets. The laser templates are designed for a stock material of 600 mm x 900 mm.

I designed the laser templates with a 0.1 mm line width for vector cutting and solid black fills for engraving. To easier separate engraving ad vector cutting, I also used red (255, 0, 0) as color for cutting lines.
This directory currently contains 3 PDF files.

### The sample
The `Wordclock_TestPiece.pdf` is a 35 mm x 35 mm sample piece that is designed to determine the lasering parameters. It contains engraving, cutting and holes in the same format as the final template. 

### The simple template
In the `Wordclock_LaserTemplate_900x600_normal.pdf` template all pieces are arranged individually. This files contains all pieces necessary to build the wordclock. In addition, individual pieces can easily be separated out and cut individually. 

Use this template, if you want to make adaptations to your template, i.e. adjust it to a different stock size.

### The optimized template
Finally, in `Wordclock_LaserTemplate_900x600_optimized.pdf` I arranged all pieces with no separation in between and removed overlapping or double lines. This reduces the process time on the laser cutter, as an individual cut is shared between two pieces. 

However, due to this optimization, the individual shapes are not complete anymore and it is not recommended to base adaptations in this file.