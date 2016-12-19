//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//NodeLockers.cc

#include "NodeLockers.h"
#include "domain/domain/Domain.h"


//Constraints.
#include <domain/constraints/SFreedom_Constraint.h>



#include "utility/actor/actor/MovableString.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/ID.h"

//! @brief Constructor por defecto.
XC::NodeLockers::NodeLockers(Mesh *owr)
  : EntCmd(owr), MovableObject(0),  tag(0), code("nil") {}

//! @brief Devuelve un puntero al bloqueador cuyo nombre se pasa como parámetro.
XC::NodeLocker *XC::NodeLockers::busca_node_locker(const std::string &nmb)
  {
    NodeLocker *retval= nullptr;
    map_node_lockers::iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      retval= i->second;
    return retval;
  }

//! @brief Devuelve un puntero al bloqueador cuyo nombre se pasa como parámetro.
const XC::NodeLocker *XC::NodeLockers::busca_node_locker(const std::string &nmb) const
  {
    const XC::NodeLocker *retval= nullptr;
    map_node_lockers::const_iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      retval= i->second;
    return retval;
  }

//! @brief Devuelve un puntero al bloqueador cuyo tag se pasa como parámetro.
XC::NodeLocker *XC::NodeLockers::buscaNodeLocker(const int &tag)
  {
    NodeLocker *retval= nullptr;
    for(map_node_lockers::iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      if(i->second->getTag()==tag)
        {
          retval= i->second;
          break;
        }
    return retval;
  }

//! @brief Devuelve un puntero al bloqueador cuyo tag se pasa como parámetro.
const XC::NodeLocker *XC::NodeLockers::buscaNodeLocker(const int &tag) const
  {
    NodeLocker *retval= nullptr;
    for(map_node_lockers::const_iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      if(i->second->getTag()==tag)
        {
          retval= i->second;
          break;
        }
    return retval;
  }

XC::NodeLocker *XC::NodeLockers::nuevoNodeLocker(const std::string &nmb)
  {
    NodeLocker *retval= nullptr;
    if(!nmb.empty())
      {
        retval= busca_node_locker(nmb);
        if(!retval)
          {
            retval= new NodeLocker(tag);
            if(retval)
              {
                node_lockers[nmb]= retval;
                tag++;
              }
            else
              std::cerr << "NodeLockers::nuevoNodeLocker; no se pudo crear: '"
                        << nmb << "'\n";
          }
        else
	  std::clog << "NodeLockers::nuevoNodeLocker; ya existe: '"
                    << nmb << "'\n";
      }
    else
      std::cerr << "NodeLockers::nuevoNodeLocker; se pasó un nombre vacío." << std::endl;
    return retval;
  }

void XC::NodeLockers::borraNodeLocker(const std::string &nmb)
  {
    map_node_lockers::iterator i= node_lockers.find(nmb);
    if(i!=node_lockers.end())
      {
        delete (*i).second;
        node_lockers.erase(i);
      }
    else
      std::cerr << "NodeLockers::borraNodeLocker; no se encontró: '"
                << nmb << "'\n";
  }

void XC::NodeLockers::removeFromDomain(const std::string &cod)
  {
    NodeLocker *p= busca_node_locker(cod);
    if(p)
      getDomain()->removeNodeLocker(p);
    else
      {
        std::cerr << "removeFromDomain: no se encontró la hipótesis: " 
                  << cod << std::endl;
      }
  }

void XC::NodeLockers::removeAllFromDomain(void)
  {
    std::deque<std::string> nombres;
    for(map_node_lockers::iterator i= node_lockers.begin();i!=node_lockers.end();i++)
      nombres.push_back((*i).first);
    for(std::deque<std::string>::const_iterator i= nombres.begin();i!=nombres.end();i++)
      removeFromDomain(*i);
  }

//! @brief Borra todos los objetos.
void XC::NodeLockers::clearAll(void)
  {
    removeAllFromDomain();
    for(map_node_lockers::iterator i= node_lockers.begin();i!= node_lockers.end();i++)
      delete (*i).second;
    node_lockers.erase(node_lockers.begin(),node_lockers.end());
  }

//! @brief Destructor.
XC::NodeLockers::~NodeLockers(void)
  { clearAll(); }

const std::string XC::NodeLockers::getCurrentNodeLockerId(void) const
  { return code; }

XC::NodeLocker *XC::NodeLockers::getCurrentNodeLockerPtr(void)
  { return busca_node_locker(code); }

const XC::NodeLocker *XC::NodeLockers::getCurrentNodeLockerPtr(void) const
  { return busca_node_locker(code); }

//! @brief Devuelve el dominio.
XC::Domain *XC::NodeLockers::getDomain(void)
  {
    Domain *retval= nullptr;
    Mesh *mesh= dynamic_cast<Mesh *>(Owner());
    if(mesh)
      retval= mesh->getDomain();
    else
      std::cerr << "(NodeLockers) necesito una malla." << std::endl;
    return retval;
  }

//! @brief Devuelve el dominio.
const XC::Domain *XC::NodeLockers::getDomain(void) const
  {
    const Domain *retval= nullptr;
    const Mesh *mesh= dynamic_cast<const Mesh *>(Owner());
    if(mesh)
      retval= mesh->getDomain();
    else
      std::cerr << "(NodeLockers) necesito una malla." << std::endl;
    return retval;
  }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodeLockers::getDbTagData(void) const
  {
    static DbTagData retval(5);
    return retval;
  }

//! @brief Send members through the channel being passed as parameter.
int XC::NodeLockers::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    const size_t sz= node_lockers.size();      
    setDbTagDataPos(1,sz);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        DbTagData objData(sz);
        int loc= 0;
        for(std::map<std::string,NodeLocker *>::iterator i=node_lockers.begin();i!=node_lockers.end();i++,loc++)
          {
            res+= cp.sendString((*i).first,labelData,CommMetaData(loc));
            res+= cp.sendMovable(*(*i).second,objData,CommMetaData(loc));
          }
        res+= labelData.send(getDbTagData(),cp,CommMetaData(2));
        res+= objData.send(getDbTagData(),cp,CommMetaData(3));
      }
    res+= cp.sendString(code,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives members through the channel being passed as parameter.
int XC::NodeLockers::recvData(const CommParameters &cp)
  {
    tag= getDbTagDataPos(0);
    const size_t sz= getDbTagDataPos(1);
    int res= 0;
    if(sz>0)
      {
        DbTagData labelData(sz);
        res+= labelData.receive(getDbTagData(),cp,CommMetaData(2));
        DbTagData objData(sz);
        res+= objData.receive(getDbTagData(),cp,CommMetaData(3));
        std::string label;
        NodeLocker *tmp= nullptr;
        for(size_t i= 0;i<sz;i++)
          {
            res+= cp.receiveString(label,labelData,CommMetaData(i));
            tmp= nuevoNodeLocker(label);
            if(tmp)
              res+= cp.receiveMovable(*tmp,objData,CommMetaData(i));
            else
	      std::cerr << "Error en NodeLockers::recvData label= " 
                        << label << std::endl;
          }
      }
    res+= cp.receiveString(code,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::NodeLockers::sendSelf(CommParameters &cp)
  {
    inicComm(5);
    int result= sendData(cp);

    const int dataTag= getDbTag();
    result+= cp.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << "NodeLockers::sendSelf() - ch failed to send data\n";
    return result;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::NodeLockers::recvSelf(const CommParameters &cp)
  {
    inicComm(5);

    const int dataTag = this->getDbTag();  
    int result = cp.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << "NodeLockers::sendSelf() - ch failed to receive data\n";
    else
      result+= recvData(cp);
    return result;    
  }
