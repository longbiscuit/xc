//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
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
//python_interface.tcc

class_<XC::TrussStrainLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("TrusStrainLoad", no_init)
  .add_property("eps1", make_function( &XC::TrussStrainLoad::E1, return_value_policy<return_by_value>() ),&XC::TrussStrainLoad::setE1)
  .add_property("eps2", make_function( &XC::TrussStrainLoad::E2, return_value_policy<return_by_value>() ),&XC::TrussStrainLoad::setE2)
  ;

class_<XC::BeamLoad, bases<XC::ElementBodyLoad>, boost::noncopyable >("BeamLoad", no_init)
  .add_property("category", &XC::BeamLoad::Categoria)
  ;

class_<XC::BeamStrainLoad, bases<XC::BeamLoad>, boost::noncopyable >("BeamStrainLoad", no_init)
  .add_property("planoDeformacionDorsal", make_function( &XC::BeamStrainLoad::getDeformationPlane1, return_internal_reference<>() ),&XC::BeamStrainLoad::setDeformationPlane1)
  .add_property("planoDeformacionFrontal", make_function( &XC::BeamStrainLoad::getDeformationPlane2, return_internal_reference<>() ),&XC::BeamStrainLoad::setDeformationPlane2)
  ;

class_<XC::BeamMecLoad, bases<XC::BeamLoad>, boost::noncopyable >("BeamMecLoad", no_init)
  .add_property("axialComponent", &XC::BeamMecLoad::getAxialComponent, &XC::BeamMecLoad::setAxialComponent)
  .add_property("transComponent", &XC::BeamMecLoad::getTransComponent, &XC::BeamMecLoad::setTransComponent)
  .def("getLocalForces",make_function(&XC::BeamMecLoad::getLocalForces, return_internal_reference<>() ))
  .def("getLocalMoments",make_function(&XC::BeamMecLoad::getLocalMoments, return_internal_reference<>() ))
  .def("getGlobalVectors",make_function(&XC::BeamMecLoad::getGlobalVectors, return_internal_reference<>() ))
  .def("getGlobalForces",make_function(&XC::BeamMecLoad::getGlobalForces, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  .def("getGlobalMoments",make_function(&XC::BeamMecLoad::getGlobalMoments, return_internal_reference<>() ),"Returns force vector(s) in global coordinates.")
  ;

class_<XC::BeamPointLoad, bases<XC::BeamMecLoad>, boost::noncopyable >("BeamPointLoad", no_init)
  .add_property("x", &XC::BeamPointLoad::X, &XC::BeamPointLoad::setX)
  ;

class_<XC::BeamUniformLoad, bases<XC::BeamMecLoad>, boost::noncopyable >("BeamUniformLoad", no_init)
  ;

class_<XC::Beam2dPointLoad, bases<XC::BeamPointLoad>, boost::noncopyable >("Beam2dPointLoad", no_init)
  ;

class_<XC::Beam2dUniformLoad, bases<XC::BeamUniformLoad>, boost::noncopyable >("Beam2dUniformLoad", no_init)
  ;


class_<XC::Beam3dPointLoad, bases<XC::BeamPointLoad>, boost::noncopyable >("Beam3dPointLoad", no_init)
  .add_property("transYComponent", &XC::Beam3dPointLoad::getTransComponent, &XC::Beam3dPointLoad::setTransComponent)
  .add_property("transZComponent", &XC::Beam3dPointLoad::getTransZComponent, &XC::Beam3dPointLoad::setTransZComponent)
  ;

class_<XC::Beam3dUniformLoad, bases<XC::BeamUniformLoad>, boost::noncopyable >("Beam3dUniformLoad", no_init)
  .add_property("transZComponent", &XC::Beam3dUniformLoad::getTransComponent, &XC::Beam3dUniformLoad::setTransZComponent)
  ;


