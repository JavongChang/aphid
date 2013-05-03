Match Shape

build 0.6

date May 3 2013

Match template mesh (left) to target mesh (right).

Installation:

copy aphid dir to anywhere

File Menu:

Load Template - load mesh (.m) to left
Load Target - load mesh (.m) to right
Save Templete - save left to mesh (.m)

Shutcut:

A - anchor select in active window
Z - remove last anchor in active window
D - anchor deform left
F - fit left to right
S - smooth left when template is bigger than target
Up - camera forward
Down - camera backward

Camera:

Alt + LMB - tumble
Alt + MMB - pan
Alt + RMB - zoom
O switch between orhographic and perspective camera
H reset camera to original pose
G view all

Select:

Click on mesh to select closest vertex. Select another point will find a (edge) path on mesh to connect last point to it.
Hold Shift to disable auto connecting and enable point on mesh select, this feature is target-only.
Space to remove selection.
] to grow selection along polygon edge, at lease 2 vertices should be selected
[ to shrink selection
Anchor on the left should have same direction as anchor on the right.

Model Maya In:

opiumPull -m "model" -p "D:/foo.m"

Model Maya Out:

opiumPush -m "model" -ia on -p "D:/foo.m"

 