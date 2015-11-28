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
//DiagInteraccion.h

#ifndef DIAG_INTERACCION2D_H
#define DIAG_INTERACCION2D_H

#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

namespace XC {

class Vector;
class FiberSectionBase;
class DatosDiagInteraccion;

//! \@ingroup MATSCCDiagInt
//
//! @brief Diagrama de interacción (N,My) de una sección.
class DiagInteraccion2d: public Poligono2d
  {
  protected:
    Pos2d get_interseccion(const Pos2d &p) const;
  public:
    DiagInteraccion2d(void);
    DiagInteraccion2d(const Poligono2d &);
    virtual DiagInteraccion2d *clon(void) const;

    void Simplify(void);
    double FactorCapacidad(const Pos2d &esf_d) const;
    Vector FactorCapacidad(const GeomObj::list_Pos2d &lp) const;

    void Print(std::ostream &os) const;
  };

DiagInteraccion2d calc_diag_interaccionPlano(const FiberSectionBase &scc,const DatosDiagInteraccion &, const double &);
DiagInteraccion2d calc_diag_interaccionNMy(const FiberSectionBase &scc,const DatosDiagInteraccion &);
DiagInteraccion2d calc_diag_interaccionNMz(const FiberSectionBase &scc,const DatosDiagInteraccion &);

} // fin namespace XC

#endif