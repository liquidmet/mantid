//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidKernel/ListValidator.h"
#include <algorithm>

namespace Mantid
{
namespace Kernel
{

/// Default constructor. Sets up an empty list of valid values.
ListValidator::ListValidator() : IValidator<std::string>(), m_allowedValues()
{}
	
/** Constructor
 *  @param values A vector of strings containing the valid values 
 */
ListValidator::ListValidator(const std::vector<std::string>& values) : 
  IValidator<std::string>(),
  m_allowedValues(values.begin(),values.end())
{}
  
/// Destructor
ListValidator::~ListValidator() {}

/// Returns the set of valid values
const std::vector<std::string>& ListValidator::allowedValues() const
{
  return m_allowedValues;
}
  
/// Adds the argument to the set of valid values
void ListValidator::addAllowedValue(const std::string &value)
{
  m_allowedValues.push_back(value);
}

IValidator<std::string>* ListValidator::clone() { return new ListValidator(*this); }
  
/** Checks if the string passed is in the list
 *  @param value The value to test
 *  @return "" if the value is on the list, or "The value is not in the list of allowed values"
 */
std::string ListValidator::checkValidity(const std::string &value) const
{
  if ( std::find(m_allowedValues.begin(),m_allowedValues.end(),value) != m_allowedValues.end() )
  {
    return "";
  }
  else
  {
    if ( value.empty() ) return "Select a value";
    else return "The value \"" + value + "\" is not in the list of allowed values";
  }
} 

} // namespace Kernel
} // namespace Mantid
