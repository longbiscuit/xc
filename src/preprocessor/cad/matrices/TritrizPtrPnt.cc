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
//TritrizPtrPnt.cc

#include "TritrizPtrPnt.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "xc_basic/src/funciones/algebra/integ_num.h"

#include <boost/any.hpp>
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Poligono3d.h"
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/cad/Cad.h"
#include "xc_utils/src/geom/pos_vec/RangoTritriz.h"
#include "xc_basic/src/matrices/RangoMatriz.h"


//! @brief Constructor por defecto.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas)
  : TritrizPtrBase<MatrizPtrPnt>(capas) {}

//! @brief Constructor.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrPnt>(capas,filas,cols) {}

void XC::TritrizPtrPnt::setPnt(const size_t &i,const size_t &j,const size_t &k,const int &id_punto)
  {
    if(check_range(i,j,k))
      {
        Cad *c= getCad();
        Pnt *tmp= TritrizPtrPnt::operator()(i,j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << i << "," << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->GetNombre() << std::endl;
        TritrizPtrPnt::operator()(i,j,k)= c->getPuntos().busca(id_punto);
      }
    else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << i << ','  << j << ',' << k << ") out of range;"
               << " number of layers: " << GetCapas() << " number of rows: " << getNumFilas() << " number of columns: " << getNumCols()
               << std::endl;
  }

XC::Pnt *XC::TritrizPtrPnt::getPnt(const size_t &i,const size_t &j,const size_t &k)
  { return getAtIJK(i,j,k); }

//! @brief Devuelve el centroide del esquema.
Pos3d XC::TritrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t ncapas= GetCapas();
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        Pos3d p= capa.getCentroide();
        x+= p.x();
        y+= p.y();
        z+= p.z(); 
      }
    x/=ncapas; y/=ncapas; z/=ncapas;
    retval= Pos3d(x,y,z);
    return retval;
  }


//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
XC::Pnt *XC::TritrizPtrPnt::buscaPunto(const int &tag)
  {
    Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        retval= capa.buscaPunto(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::TritrizPtrPnt::getCad(void) const
  {
    const Cad *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas3d *e3d= dynamic_cast<const MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getCad();
    assert(retval);
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::TritrizPtrPnt::getCad(void)
  {
    Cad *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas3d *e3d= dynamic_cast<MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getCad();
    assert(retval);
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto being passed as parameter.
const XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    TritrizPtrPnt *this_no_const= const_cast<TritrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Devuelve el punto más próximo al punto being passed as parameter.
XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        ptrPnt= capa.getNearestPnt(p);
        if(ptrPnt)
          {
            tmp= ptrPnt->DistanciaA2(p);
            if(tmp<d)
              {
                d= tmp;
                retval= ptrPnt;
              }
          }
      }
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
const XC::Pnt *XC::TritrizPtrPnt::buscaPunto(const int &tag) const
  {
    const Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        retval= capa.buscaPunto(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Copia los puntos del rango being passed as parameter, colocándolos
//! en las posiciones de la matriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir (i,j)->(i+offsetIndices[0],j+offsetIndices[1])
//! y desplazando su posición geométrica según el vector vectorOffset.
std::deque<size_t> XC::TritrizPtrPnt::CopiaPuntos(const RangoTritriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    Cad *cad= getCad();
    std::deque<size_t> retval;
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
      for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
        for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
          {
            const Pnt *p= operator()(i,j,k);
            if(p)
              {
                Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
                (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= nuevo;
                retval.push_back(nuevo->GetTag());
              }
          }
    return retval;
  }  

//! @brief Devuelve los puntos del rango being passed as parameter.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getRangoPuntos(const RangoTritriz &rango)
  {
    TritrizPtrPnt retval(rango.NumCapas(),rango.NumFilas(),rango.NumCols());
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    const size_t rcapas_inf= rcapas.Inf();
    const size_t rfilas_inf= rfilas.Inf();
    const size_t rcols_inf= rcols.Inf();
    for(size_t i= rcapas_inf;i<=rcapas.Sup();i++)
      for(size_t j= rfilas_inf;j<=rfilas.Sup();j++)
        for(size_t k= rcols_inf;k<=rcols.Sup();k++)
          {
            Pnt *p= operator()(i,j,k);
            if(p)
              retval(i-rcapas_inf+1,j-rfilas_inf+1,k-rcols_inf+1)= p;
          }
    return retval;
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::Pnt *XC::TritrizPtrPnt::getPunto(const VIndices &iPunto)
  {
    Pnt *retval= nullptr;
    if(iPunto.size()>2)
      {
        if((iPunto[0]>0) && (iPunto[1]>0) && (iPunto[2]>0))
          { retval= (*this)(iPunto[0],iPunto[1],iPunto[2]); }
      }
    else
      std::cerr << "TritrizPtrPnt::getPunto; el vector de índices: "
                << iPunto << " no es válido." << std::endl;
    return retval;    
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getPuntos(const TritrizIndices &indices)
  {
    const size_t nCapas= indices.GetCapas();
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    TritrizPtrPnt retval(nCapas,nFilas,nCols);
    for(size_t i= 1;i<= nCapas;i++)
      for(size_t j= 1;j<= nFilas;j++)
        for(size_t k= 1;k<= nCapas;k++)
          {
            const VIndices iPunto= indices(i,j,k);
            if(iPunto.size()>2)
              { retval(i,j,k)= getPunto(iPunto); }
            else
	      std::cerr << "TritrizPtrPnt::getPuntos; el vector de índices: "
                        << iPunto << " no es válido." << std::endl;
          }
    return retval;
  }

//! @brief Devuelve los puntos cuyos índices se pasan como parámetro.
XC::MatrizPtrPnt XC::TritrizPtrPnt::getPuntos(const MatrizIndices &indices)
  {
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    MatrizPtrPnt retval(nFilas,nCols);
    for(size_t i= 1;i<= nFilas;i++)
      for(size_t j= 1;j<= nCols;j++)
        {
          const VIndices iPunto= indices(i,j);
          if(iPunto.size()>2)
            { retval(i,j)= getPunto(iPunto); }
          else
            std::cerr << "TritrizPtrPnt::getPuntos; el vector de índices: "
                      << iPunto << " no es válido." << std::endl;
        }
    return retval;
  }

//! @brief Devuelve la celda formada por los puntos que se obtienen de las
//! posiciones de la tritriz que resultan de sumar a los índices (i,j,k) del
//! punto los valores del vector offsetIndices es decir:
//! Punto (i,j,k): (i+offsetIndices(i,j,k)[0],j+offsetIndices(i,j,k)[1],k+offsetIndices(i,j,k)[2])
XC::TritrizPtrPnt XC::TritrizPtrPnt::getCeldaPuntos(const size_t &i,const size_t &j,const size_t &k,const TritrizIndices &offsetIndices)
  {
    VIndices org(3);
    org[0]= i;org[1]= j;org[2]= k;
    TritrizIndices tmp(offsetIndices);
    tmp.Offset(org);
    return getPuntos(tmp);
  }

//! @brief Devuelve la celda formada por los puntos que se obtienen de las
//! posiciones de la tritriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir:
//! Punto (i,j): (i+offsetIndices(i,j)[0],j+offsetIndices(i,j)[1],k+offsetIndices(i,j)[2])
XC::MatrizPtrPnt XC::TritrizPtrPnt::getCeldaPuntos(const size_t &i,const size_t &j,const MatrizIndices &offsetIndices)
  {
    VIndices org(2);
    org[0]= i;org[1]= j;
    MatrizIndices tmp(offsetIndices);
    tmp.Offset(org);
    return getPuntos(tmp);
  }

// //! @brief Crea superficies cuadriláteras entre los los puntos del rango being passed as parameter, colocándolos
// //! entre las posiciones de la tritriz que resultan de sumar a los índices (i,j) del
// //! punto los valores del vector offsetIndices es decir:
// //! Punto 1: (i+offsetIndices[0,0],j+offsetIndices[0,1],k+offsetIndices[0,2])
// //! Punto 2: (i+offsetIndices[1,0],j+offsetIndices[1,1],k+offsetIndices[1,2])
// //! Punto 3: (i+offsetIndices[2,0],j+offsetIndices[2,1],k+offsetIndices[2,2])
// //! ...
// //! @param nf: Número de filas de la matriz de apuntadores a punto.
// //! @param nc: Número de columnas de la matriz de apuntadores a punto.
// std::deque<size_t> XC::TritrizPtrPnt::CreaCuadrilateros(const RangoTritriz &rango,const size_t &nf,const size_t &nc,const m_int &offsetIndices,const double &elemSizeI,const double &elemeSizeJ)
//   {
//     Cad *cad= getCad();
//     std::deque<size_t> retval;
//     const RangoIndice &rcapas= rango.GetRangoCapas();
//     const RangoIndice &rfilas= rango.GetRangoFilas();
//     const RangoIndice &rcols= rango.GetRangoCols();
//     for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
//       for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
//         for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
//           {
//             const Pnt *p= operator()(i,j,k);
//             if(p)
//               {
//                 Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
//                 (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= nuevo;
//                 retval.push_back(nuevo->GetTag());
//               }
//           }
//     return retval;
//   }  

void XC::TritrizPtrPnt::Print(std::ostream &os) const
  {
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t i=1;i<=ncapas;i++)
      {
        for(size_t j=1;j<=nfilas;j++)
          {
            for(size_t k=1;k<=ncols;k++)
	      os << (*this)(i,j,k)->GetTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const TritrizPtrPnt &t)
  {
    t.Print(os);
    return os;
  }

//! @brief Devuelve los índices de los puntos (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<size_t> XC::getIdPuntosQuad(const TritrizPtrPnt::const_ref_capa_i_cte &puntos,const size_t &j,const size_t &k)
  {
    std::vector<size_t> retval(4,-1);
    const size_t nfilas= puntos.getNumFilas();
    const size_t ncols= puntos.getNumCols();
    if(j>=nfilas)
      {
        std::cerr << "getIdPuntosQuad; índice de fila j= " << j << " fuera de rango.\n";
        return retval;
      }
    if(k>=ncols)
      {
        std::cerr << "getIdPuntosQuad; índice de columna k= " << k << " fuera de rango.\n";
        return retval;
      }


    Pos3d p1;
    const Pnt *ptr= puntos(j,k);
    if(ptr)
      {
        retval[0]= ptr->GetTag();
        if(retval[0]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j << ',' << k << ").\n";
        p1= ptr->GetPos();
      }

    Pos3d p2;
    ptr= puntos(j,k+1);
    if(ptr)
      {
        retval[1]= ptr->GetTag();
        if(retval[1]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j << ',' << k+1 << ").\n";
        p2= ptr->GetPos();
      }

    Pos3d p3;
    ptr= puntos(j+1,k+1);
    if(ptr)
      {
        retval[2]= ptr->GetTag();
        if(retval[2]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j+1 << ',' << k+1 << ").\n";
        p3= ptr->GetPos();
      }

    Pos3d p4;
    ptr= puntos(j+1,k);
    if(ptr)
      {
        retval[3]=ptr->GetTag();
        if(retval[3]<0)
          std::cerr << "getIdPuntosQuad; error al obtener el identificador de punto (" << j+1 << ',' << k << ").\n";
        p4= ptr->GetPos();
      }

//     const Vector3d v2= p2-p1;
//     const Vector3d v3= p3-p2;
//     const Vector3d v4= p4-p3;
//     const Vector3d v1= p1-p4;
//     const double d1= dot(v1,v3);
//     const double d2= dot(v2,v4);
//     if((d1>0))
//       {
//         std::swap(p3,p2);
//         std::swap(retval[2],retval[1]);
//       }
//     if((d2>0))
//       {
// 	std::swap(p3,p4);
//         std::swap(retval[2],retval[3]);
//       }

    std::list<Pos3d> posiciones;
    posiciones.push_back(p1);
    posiciones.push_back(p2);
    posiciones.push_back(p3);
    posiciones.push_back(p4);
    Poligono3d tmp(posiciones.begin(),posiciones.end());
    const double area= tmp.Area();
    if(area<1e-3)
      {
        std::cerr << "Al obtener la celda de índices (" << j << ',' << k
                  << ") se obtuvo un área muy pequeña (" << area << ").\n";
        std::cerr << " posición del punto (j,k) " << p1 << std::endl;
	std::cerr << " posición del punto (j+1,k) " << p2 << std::endl;
	std::cerr << " posición del punto (j+1,k+1) " << p3 << std::endl;
	std::cerr << " posición del punto (1,k+1) " << p4 << std::endl;
      }
    return retval;
  }

