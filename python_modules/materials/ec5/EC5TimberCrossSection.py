# -*- coding: utf-8 -*-

from __future__ import division
import logging
from materials import parametrosSeccionRectangular
from postprocess import def_vars_control
from postprocess import callback_controls
from postprocess import prop_statistics as ps

logging.addLevelName( logging.WARNING, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.WARNING))
logging.addLevelName( logging.ERROR, "\033[1;41m%s\033[1;0m" % logging.getLevelName(logging.ERROR))

class EC3TimberRectCrossSection(parametrosSeccionRectangular.RectangularSection):
  """IPE profile with Eurocode 3 verification routines."""
  def __init__(self,name,b,h,E,nu,fyd,taud):
    super(EC3TimberRectCrossSection,self).__init__(name,b,h,E,nu)
    self.fyd= fyd
    self.taud= taud

  def setupULSControlVars2d(self,elems):
    '''Creates control variables for ULS in elems.'''
    def_vars_control.defVarsControlTensRegElastico2d(elems)
    W= self.Wzel()
    for e in elems:
      e.setProp("fyd",self.fyd)
      e.setProp("fydV",self.taud)
      e.setProp("Wel",W)

  def setupULSControlVars3d(self,elems):
    '''Creates control variables for ULS in elems.'''
    def_vars_control.defVarsControlTensRegElastico3d(elems)
    Wz= self.Wzel()
    Wy= self.Wyel()
    for e in elems:
      e.setProp("fyd",self.fyd)
      e.setProp("fydV",self.taud)
      e.setProp("Wyel",Wy)
      e.setProp("AreaQy",0.9*self.A())
      e.setProp("Wzel",Wz)
      e.setProp("AreaQz",self.A()-e.getProp("AreaQy"))

  def installElementElasticStressesControlRecorder(self,recorderName, elemSet):
    preprocessor= elemSet.owner.getPreprocessor
    nodes= preprocessor.getNodeLoader
    domain= preprocessor.getDomain
    recorder= domain.newRecorder(recorderName,None);
    recorder.setElements(elemSet.getTags())
    if(nodes.numGdls==3):
      self.setupULSControlVars2d(elemSet)
      recorder.callbackRecord= callback_controls.controTensRecElastico2d()
    else:
      self.setupULSControlVars3d(elemSet)
      recorder.callbackRecord= callback_controls.controTensRecElastico3d()

    recorder.callbackRestart= "print \"Restart method called.\""
    return recorder

def printResultsELU(elems,crossSection):
  '''print ULS results.'''
  fmt= "{:6.1f}"
  fmt2= "{:5.2f}"
  e= ps.getItemWithMaxProp(elems,"getProp",'FCTNCP')
  N= fmt.format(e.getProp("NCP")/1e3)
  My= fmt.format(e.getProp("MyCP")/1e3)
  Mz= fmt.format(e.getProp("MzCP")/1e3)
  sgMax= fmt.format(e.getProp("SgMax")/1e6)
  sgMin= fmt.format(e.getProp("SgMin")/1e6)
  sgAdm= fmt.format(crossSection.fyd/1e6)
  fctnCP= fmt2.format(e.getProp("FCTNCP"))
  print "tag= ", e.tag, " N= ", N , "kN  My= ", My , "kN.m  Mz= ", Mz , "kN-m   SgMax= ", sgMax , "MPa  SgMin= ", sgMin, "MPa  sgAdm= ", sgAdm , "MPa  FCTNCP= ", fctnCP , "HIPCPTN= ",e.getProp("HIPCPTN")
  e=  ps.getItemWithMaxProp(elems,"getProp",'FCVCP')
  Vy= fmt.format(e.getProp("VyCP")/1e3)
  Vz= fmt.format(e.getProp("VzCP")/1e3)
  tauMax=  fmt.format(e.getProp("TauMax")/1e6)
  tauAdm= fmt.format(crossSection.taud/1e6)
  fcvCP= fmt2.format(e.getProp("FCVCP"))
  print "tag= ", e.tag, " Vy= ", Vy , "kN  Vz= ", Vz , "kN  TauMax= ", tauMax , "MPa tauAdm= ", tauAdm, "MPa FCVCP= ", fcvCP , " HIPCPV= ", e.getProp("HIPCPV")