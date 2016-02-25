# -*- coding: utf-8 -*-
from __future__ import division

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AOO)"
__cppyright__= "Copyright 2016, AOO and LCPT"
__license__= "GPL"
__version__= "1.0"
__email__= "l.pereztato@gmail.com  ana.Ortega.Ort@gmail.com"

import math

class ArchBridgeRoughModel:
  ''' Arch bridge simple model'''
  def __init__(self,L,d):
    self.d= d # rise (depth) of the arch at midspan
    self.L= L # horizontal distance between supports

  #*Uniformly distributed loads
  #Abutment reactions
  def getQunfVabtm(self,qunif):
    '''Vertical reaction at each abutment due to a uniform load.
    Attributes:
       qunif: uniformly distributed load applied to the arch
    '''
    return qunif*self.L/2.0
  def getQunfHabtm(self,qunif):
    '''Horizontal reaction at each abutment due to a uniform load.
    Attributes:
       qunif: uniformly distributed load applied to the arch
    '''
    return qunif*self.L**2/8.0/self.d
  #Compressive stress
  def getQunfCompStress(self,qunif,A):
    '''Approximation of the compressive stress in a section of the 
    arch with area=A, due to a uniform load qunif.
    for simplicity, it's considered the compressive force at the midspan
    (H, that, in turn, i equal to the horizontal reaction at the abutment)
    as an approximation for the compressive force in the arch at any location
    '''  
    return -self.getQunfHabtm(qunif)/A  

  #*Concentrated loads in three hinged arch at each quarterpoint 
  #Abutment reactions
  def getQconcVabtm(self,Qconc):
    '''Vertical reaction at each abutment of a three hinged arch 
    due to two concentrated loads in quarterpoints
    '''
    return Qconc

  def getQconcHabtm(self,Qconc):
    '''Horizontal reaction at each abutment of a three hinged arch 
    due to a two concentrated loads in quarterpoints
    '''
    return Qconc*self.L/4.0/self.d
  #Compressive stress
  def getQconcCompStress(self,Qconc,A):
    '''Approximation of the compressive stress in a section of the 
    three hinged arch with area=A, due to a two concentrated loads in quarterpoints.
    for simplicity, it's considered the compressive force at the midspan
    (H, that, in turn, i equal to the horizontal reaction at the abutment)
    as an approximation for the compressive force in the arch at any location
    '''  
    return -self.getQconcHabtm(Qconc)/A  
  #Bending moments in a parabolic three hinged arch produced by 
  #concentrated live forces placed at quarterspans
  def getMmaxQconc(self,Qconc):
    '''maximum bending moment in a parabolic three hinged arch
    produced by concentrated live forces placed at quarterspans
    The bending moment is maximum at the quarterpoints
    '''
    MV=Qconc*self.L/4  #due to vertical reactions at abutments
    MH=-3*self.getQconcHabtm(Qconc)*self.d/4 #due to horizontal reactions at abutments
    return MV+MH

