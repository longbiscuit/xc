# -*- coding: utf-8 -*-

from __future__ import division
import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from materials import typical_materials
from model import fix_node_6dof
import math

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

b= 0.4
h= 0.8
A= b*h
E= 200000*9.81/1e-4 # Elastic modulus aproximado del hormigón.
nu= 0.3 # Poisson's ratio
G= E/(2*(1+nu)) # Shear modulus
Iy= (1/12.0*h*b**3) # Cross section moment of inertia (m4)
Iz= (1/12.0*b*h**3) # Cross section moment of inertia (m4)
J= 0.721e-8 # Cross section torsion constant (m4)
L= 1 # Longitud del elmento expressed in meters.

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.defaultTag= 1 #First node number.
nod= nodes.newNodeXYZ(0.0,0.0,0.0)
nod= nodes.newNodeXYZ(L,0.0,0)

trfs= preprocessor.getTransfCooLoader
lin= trfs.newLinearCrdTransf3d("lin")
lin.xzVector= xc.Vector([0,1,0])

# Materials
caracMecSeccion= xc.CrossSectionProperties3d()
caracMecSeccion.A= A; caracMecSeccion.E= E; caracMecSeccion.G= G;
caracMecSeccion.Iz= Iz; caracMecSeccion.Iy= Iy; caracMecSeccion.J= J
seccion= typical_materials.defElasticSectionFromMechProp3d(preprocessor, "seccion",caracMecSeccion)

# Elements definition
elementos= preprocessor.getElementLoader
elementos.defaultTransformation= "lin"
elementos.defaultTag= 1 #Tag for the next element.
elementos.defaultMaterial= "seccion"
beam3d= elementos.newElement("elastic_beam_3d",xc.ID([1,2]));


fuerte= beam3d.getVDirStrongAxisGlobalCoord()
debil= beam3d.getVDirWeakAxisGlobalCoord()
angSeccion= beam3d.getStrongAxisAngle()
ratio1= ((fuerte[0])**2+(fuerte[2])**2)
ratio2= ((debil[0])**2+(debil[1])**2)
ratio3= abs(angSeccion-math.pi/2)/(math.pi/2)



''' 
print "strong axis: ", fuerte
print "ratio1= ",ratio1
print "weak axis: ", debil
print "ratio2= ",ratio2
print "ratio3= ",ratio3
  '''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-15) & (ratio2 < 1e-15) & (ratio3 < 1e-15):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."
