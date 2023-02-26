#ifndef _NgoUnit_h
#define _NgoUnit_h
/*******************************************************************************
   FILE DESCRIPTION
*******************************************************************************/
/*!
@file NgoUnit.h
@author Cedric ROMAN - roman@numengo.com
@date February 2009
@brief File containing all prototypes related to unit manager.
 */

/*******************************************************************************
   LICENSE
*******************************************************************************
 Copyright (C) 2009 Numengo (admin@numengo.com)

 This document is released under the terms of the numenGo EULA.  You should have received a
 copy of the numenGo EULA along with this file; see  the file LICENSE.TXT. If not, write at
 admin@numengo.com or at NUMENGO, 15 boulevard Vivier Merle, 69003 LYON - FRANCE
 You are not allowed to use, copy, modify or distribute this file unless you  conform to numenGo
 EULA license.
*/


/*******************************************************************************
   REVISIONS
*******************************************************************************/

/*******************************************************************************
   INCLUDES
*******************************************************************************/
#include <iostream>
#include <sstream>
#include <string>

#include "ngocommon/NgoSingletonManager.h"
#include "ngounit/NgoUnitExports.h"
/*******************************************************************************
   DEFINES / TYPDEFS / ENUMS
*******************************************************************************/


/*! @brief function to retrieve unit conversion factor and offset
 @param iunit input unit
 @param ounit output unit
 @param o_fact conversion factor
 @param o_off conversion offset
 @ingroup GroupNgoFlCommon
*/
NGO_UNIT_EXPORT int NgoGetUnitConversion(const char * iunit, const char * ounit, double *o_fact, double *o_off);



/*******************************************************************************
   CLASS Units DECLARATION
*******************************************************************************/
/*!
@struct Dimension
@brief contains information related to a particular dimension, such as name, SI unit, reduced unit, preferred units for conversion
*/
typedef struct Dimension
{
   std::string name;
   std::string SI_unit;
   std::string default_unit;
   std::string reduced_unit;
   std::vector<std::string> avl_conv;
} Dimension;

/*! @brief Class for unit conversion */
/*! @class Units */
class NGO_UNIT_EXPORT NgoUnit : public NgoSingleton<NgoUnit>
{
   friend class NgoSingleton<NgoUnit>;
public:
   /*! @brief method to set a new unit from a based unit */
   /*! @param i_str_new_unit input new unit string */
   /*! @param i_str_base_unit input base unit string */
   /*! @param i_conv_factor input conversion factor new=X*base */
   /*! @param _conv_offset input conversion offset new=X*base+Y*/
   void setConversion(
   const char * i_str_new_unit,
   const char * i_str_base_unit,
   const double i_conv_factor,
   const double i_conv_offset=0.
   );

   /*! @brief method to convert a unit to another */
   /*! @param i_str_from_unit input unit string to convert from */
   /*! @param i_str_to_unit input unit string to convert to */
   /*! @param o_conv_factor output conversion factor */
   /*! @param o_conv_offset output conversion offset */
   void conversion(
   const char * i_str_from_unit,
   const char * i_str_to_unit,
   double & o_conv_factor,
   double & o_conv_offset
   );

   /*! @brief method to convert a value form a unit to another */
   /*! @param input value to convert */
   /*! @param i_str_from_unit input unit string to convert from */
   /*! @param i_str_to_unit input unit string to convert to (empty string for SI) */
   /*! @param is_diff boolean to indicate if quantity is a difference (ex: of temperature, in this case, no offset)*/
   /*! @return converted value */
   double convert(
   const double i_value,
   const char * i_str_from_unit,
   const char * i_str_to_unit="",
   const bool is_diff = false
   );

   /*! @brief method to convert a value form a unit to another */
   /*! @param input vector of value to convert */
   /*! @param i_str_from_unit input unit string to convert from */
   /*! @param i_str_to_unit input unit string to convert to (empty string for SI)*/
   /*! @param is_diff boolean to indicate if quantity is a difference (ex: of temperature, in this case, no offset)*/
   /*! @return converted vector of values */
   std::vector<double> convert(
   const std::vector<double> & i_value,
   const char * i_str_from_unit,
   const char * i_str_to_unit="",
   const bool is_diff = false
   );

   /*! @brief method to get possible conversion unit list */
   /*! @param i_str_expr input unit string */
   /*! @return ouput list of possible conversions */
   std::vector<std::string> listUnits(
   const char * i_str_expr
   );

   /*! @brief method to clean an expression */
   /*! make an input string in the format num1-num2-...-numN1/den1-den2-...denN2 */
   /*! @param i_str_expr input unit string */
   /*! @return unit format cleaned */
   std::string clean(const char * i_str_expr);

   /*! @brief method to multiply two unit expression */
   /*! make an input string in the format num1-num2-...-numN1/den1-den2-...denN2 */
   /*! @param i_str_expr1 input unit 1 string */
   /*! @param i_str_expr2 input unit 2 string */
   /*! @return equivalent unit of the multiplication */
   std::string multiply(
   const char * i_str_expr1,
   const char * i_str_expr2
   );

   /*! @brief method to inverse an expression */
   /*! make an input string in the format num1-num2-...-numN1/den1-den2-...denN2 */
   /*! @param i_str_expr input unit string */
   /*! @return equivalent unit of the inversion */
   std::string inverse(const char * i_str_expr);

   /*! @brief method to reduce unit in base units */
   /*! @param i_str_expr input unit string */
   /*! @return equivalent unit in based units */
   std::string reduce(const char * i_str_expr);

protected:
   /*! @brief method to split a unit in 2 vectors of substrings with 2 vectors of corresponding exponents */
   /*! @param i_str_expr input unit string */
   /*! @param num vector of strings of unit at numerator */
   /*! @param num_exp vector of strings of exponents values at numerator */
   /*! @param den vector of strings of unit at numerator */
   /*! @param den_exp vector of strings of exponents values at numerator */
   void split(
   const char * i_str_expr,
   std::vector<std::string> & num,
   std::vector<std::string> & num_exp,
   std::vector<std::string> & den,
   std::vector<std::string> & den_exp
   );

   std::string smartPrint(
   std::vector<std::string> num,
   std::vector<std::string> num_exp,
   std::vector<std::string> den,
   std::vector<std::string> den_exp
   );
private:
   /*! @brief vector of available dimensions */
   std::vector<Dimension> dimension;
protected:
   /*! @brief Constructor */
   NgoUnit();
   /*! @brief Destructor */
   ~NgoUnit();
};

/*! Computes the maximum of \a a and \a b */
#ifndef MAX
#define MAX(a, b)       ((a) < (b) ? (b) : (a))
#endif

#endif
