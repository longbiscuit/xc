# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

import NodeProperties as ncd
import EPPlaneProperties as eppp
import NodeSupportProperties as nsp
import LoadGroupProperties as lgp
import LoadCaseProperties as lcp
import LoadCombProperties as lcmbp
import NodeLoadProperties as nlp
import ElementLoadProperties as elp
import xml.etree.cElementTree as ET

class ProjectProperties(object):    
    
  def __init__(self,xmlns= 'http://www.scia.cz', fileName= ''):
    self.xmlns= xmlns
    self.fileName= fileName
    self.nodeProperties= ncd.NodeProperties()
    self.epPlaneProperties= eppp.EPPlaneProperties()
    self.nodeSupportProperties= nsp.NodeSupportProperties()
    self.loadGroupProperties= lgp.LoadGroupProperties()
    self.loadCaseProperties= lcp.LoadCaseProperties()
    self.loadCombProperties= lcmbp.LoadCombProperties()
    self.nodeLoadProperties= nlp.NodeLoadProperties()
    self.elementLoadProperties= elp.ElementLoadProperties()

  def getXMLElement(self,defFileName):
    project= ET.Element("def_project")
    project.set("xmlns",self.xmlns)
    containers= [self.nodeProperties,self.epPlaneProperties, self.nodeSupportProperties, self.loadGroupProperties, self.loadCaseProperties, self.loadCombProperties, self.nodeLoadProperties, self.elementLoadProperties]
    for c in containers:
      elem= c.getXMLElement(project)
    return project

  def getXMLTree(self,defFileName):
    project= self.getXMLElement(defFileName)
    tree = ET.ElementTree(project)
    return tree

  def writeXMLFile(self): 
    tree= self.getXMLTree(self.fileName)
    tree.write(self.fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")