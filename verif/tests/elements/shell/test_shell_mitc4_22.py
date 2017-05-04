# -*- coding: utf-8 -*-
# home made test

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__copyright__= "Copyright 2015, LCPT and AOO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
import xc
from solution import predefined_solutions
from model import predefined_spaces
from model import fix_node_6dof
from materials import typical_materials

E= 2.1e6 # Young modulus of the steel.
nu= 0.3 # Poisson's ratio.
h= 0.1 # Espesor.
dens= 1.33 # Densidad kg/m2.

q= 1

# Problem type
prueba= xc.ProblemaEF()
preprocessor=  prueba.getPreprocessor
nodes= preprocessor.getNodeLoader
modelSpace= predefined_spaces.StructuralMechanics3D(nodes)
nodes.newNodeIDXYZ(1,0,0,0)
nodes.newNodeIDXYZ(2,1,0,0)
nodes.newNodeIDXYZ(3,1,1,0)
nodes.newNodeIDXYZ(4,0,1,0)


# Materials definition

memb1= typical_materials.defElasticMembranePlateSection(preprocessor, "memb1",E,nu,dens,h)


elementos= preprocessor.getElementLoader
elementos.defaultMaterial= "memb1"
elem= elementos.newElement("shell_mitc4",xc.ID([1,2,3,4]))


# Constraints
coacciones= preprocessor.getConstraintLoader

fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 1)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 2)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 3)
fix_node_6dof.Nodo6DOFGirosLibres(coacciones, 4)

# Loads definition
cargas= preprocessor.getLoadLoader

casos= cargas.getLoadPatterns

#Load modulation.
ts= casos.newTimeSeries("constant_ts","ts")
casos.currentTimeSeries= "ts"
#Load case definition
lp0= casos.newLoadPattern("default","0")
casos.currentLoadPattern= "0"


mesh= prueba.getDomain.getMesh
loadVector= xc.Vector([0.0,0.0,q])
eIter= mesh.getElementIter
elem= eIter.next()
while not(elem is None):
  elementLoad= elem.vector3dUniformLoadGlobal(loadVector)
  elem= eIter.next()

#We add the load case to domain.
casos.addToDomain("0")


# Solution
analisis= predefined_solutions.simple_static_linear(prueba)
result= analisis.analyze(1)


nodes.calculateNodalReactions(True)
nodes= preprocessor.getNodeLoader

RN1= nodes.getNode(1).getReaction[2] 
RN2= nodes.getNode(2).getReaction[2] 
RN3= nodes.getNode(3).getReaction[2] 
RN4= nodes.getNode(4).getReaction[2] 



ratio1= (abs((RN1+(q/4.0))/(q/4.0)))
ratio2= (abs((RN2+(q/4.0))/(q/4.0)))
ratio3= (abs((RN3+(q/4.0))/(q/4.0)))
ratio4= (abs((RN4+(q/4.0))/(q/4.0)))


''' 
print "ratio1= ",ratio1
print "ratio2= ",ratio2
print "ratio3= ",ratio3
print "ratio4= ",ratio4
'''

import os
fname= os.path.basename(__file__)
if (ratio1 < 1e-12) & (ratio2 < 1e-12) & (ratio3 < 1e-12) & (ratio4 < 1e-12):
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

