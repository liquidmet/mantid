#include "MantidGeometry/Instrument/Component.h"
#include <boost/python/class.hpp>
#include <boost/python/overloads.hpp>

using Mantid::Geometry::Component;
using Mantid::Geometry::IComponent;
using namespace boost::python;

namespace
{
  // Default parameter function overloads
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_getParameterNames,Component::getParameterNames,0,1);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_hasParameter,Component::hasParameter,1,2);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_getNumberParameter,Component::getNumberParameter,1,2);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_getPositionParameter,Component::getPositionParameter,1,2);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_getRotationParameter,Component::getRotationParameter,1,2);
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Component_getStringParameter,Component::getStringParameter,1,2);
}

void export_Component()
{
  class_<Component, bases<IComponent>, boost::noncopyable>("Component", no_init)
    .def("get_parameter_names", &Component::getParameterNames, Component_getParameterNames())
    .def("has_parameter", &Component::hasParameter, Component_hasParameter())
    .def("get_number_parameter", &Component::getNumberParameter, Component_getNumberParameter())
    .def("get_position_parameter", &Component::getPositionParameter, Component_getPositionParameter())
    .def("get_rotation_parameter", &Component::getRotationParameter, Component_getRotationParameter())
    .def("get_string_parameter", &Component::getStringParameter, Component_getStringParameter())
    ;

}

