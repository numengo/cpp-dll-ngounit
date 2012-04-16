/*******************************************************************************
   FILE DESCRIPTION
*******************************************************************************/
/*!
@file NgoUnit.cpp
@author Cedric ROMAN - roman@numengo.com
@date February 2009
@brief File containing class  member definitions for unit conversion
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
   DOXYGEN GROUP DEFINION
*******************************************************************************/


/*******************************************************************************
   INCLUDES
*******************************************************************************/
#include <sstream>
#include <set>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range.hpp>

#include <xmlmgr/XmlManagerBase.h>

#include <libxml/xmlreader.h>
#include <libxml/xpath.h>

#include "ngoerr/NgoError.h"
#include "ngounit/NgoUnit.h"
#include "units.h"
#include "ngocommon/NgoFileHandler.h"
#include "ngocommon/ngocommon.h"

using std::string;
using namespace boost;
using namespace boost::algorithm;


/*******************************************************************************
   UNIT CONVERSION
*******************************************************************************/

int NgoGetUnitConversion(const char * iunit, const char * ounit, double * fact, double * off)
{
   try
   {
      NgoUnit::get()->conversion(iunit,ounit,*fact,*off);
      return 1;
   }
   catch (NgoError & er)
   {
      er.addScopeError("NgoGetUnitConversion");
      NgoProcessError(er);
      return 0;
   }
}
/*******************************************************************************
   DEFINES / TYPDEFS / ENUMS
*******************************************************************************/

static const char * dimensions[] =
{
"absorbed_dose","Gy",
"acceleration","m/s^2",
"action","J/s",
"activity","mol/s",
"amount","mol",
"angular_acceleration","rad/s^2",
"angular_momentum","J-s",
"angular_velocity","rad/s",
"area","m^2",
"capacitance","F",
"conductance","S",
"conductivity","S/m",
"current","A",
"currency","dollar",
"dipole_moment","Debye",
"dose_equivalent","Sv",
"dynamic_viscosity","Pa-s",
"electric_charge","C",
"electric_potential","V",
"energy","J",
"energy_density","J/m^3",
"force","N",
"frequency","Hz",
"illuminance","lux",
"impedance","Ohm",
"inductance","H",
"kinematic_viscosity","m^2/s",
"length","m",
"luminance","cd/m^2",
"luminous_flux","lm",
"luminous_intensity","cd",
"magnetic_field intensity","A/m",
"magnetic_flux","Wb",
"magnetic_flux density","T",
"mass","kg",
"mass_flow","kg/s",
"mass_density","kg/m^3",
"molar_density","mol/m^3",
"molar_flow","mol/s",
"molar_energy","J/mol",
"molar_heat capacity","J/mol-K",
"molar_volume","m^3/mol",
"molecular_weight","g/mol",
"moment_of_inertia","kg-m^2",
"momentum","kg-m/s",
"permeability","H/m",
"permittivity","F/m",
"plane angle","rad",
"power","W",
"pressure","Pa",
"reluctance","1/H",
"resistance","Ohm",
"resistivity","Ohm-m",
"solid angle","sr",
"specific_energy","J/kg",
"specific_heat_capacity","J/kg-K",
"specific_volume","m^3/kg",
"stress","Pa",
"surface_density","kg/m^2",
"surface_tension","N/m",
"temperature","K",
"thermal_conductivity","W/m-K",
"time","s",
"torque","N-m",
"velocity","m/s",
"volume","m^3",
"volumetric_flow","m^3/s",
"wavenumber","1/m",
NULL, NULL
};

/*******************************************************************************
   CLASS Unit DEFINITION
*******************************************************************************/
template <> NgoUnit * NgoSingleton<NgoUnit>::instance_ = 0L;

std::string NgoUnit::reduce(const char * i_str_expr)
{
   std::string cleaned = clean(i_str_expr);
   char * c_red_unit = units_reduce((char *)(cleaned.c_str()));
   if (!c_red_unit)
   {
      char * u_msg = units_check_exception();
      string msg;
      if (u_msg)
         msg = u_msg;
      units_clear_exception();
      std::ostringstream oss;
      oss << "Problem reducing unit : " << i_str_expr << "\n" << msg << "\n";
      throw NgoError(oss.str(),"NgoUnit::NgoUnit","numenGo");
      return  i_str_expr;
   }
   string red_unit(c_red_unit);
   size_t found = red_unit.find_last_of("0123456789+.");
   if (found != string::npos)
   {
      red_unit = red_unit.substr(found+1);
   }
   trim(red_unit);
   replace_all(red_unit," ","-");
   replace_all(red_unit,"-/-","/");
//   free(c_red_unit);
   return red_unit;
}

NgoUnit::NgoUnit()
{
   size_t i;

   try
   {
      for (i=0;dimensions[2*i]!=NULL;i++)
      {
         dimension.resize(i+1);
         dimension[i].name = dimensions[2*i];
         dimension[i].SI_unit = dimensions[2*i+1];
         dimension[i].SI_unit = NgoUnit::clean(dimension[i].SI_unit.c_str());
         dimension[i].reduced_unit = dimensions[2*i+1];
         dimension[i].reduced_unit = NgoUnit::reduce(dimension[i].reduced_unit.c_str());
         dimension[i].default_unit = dimension[i].SI_unit;
         dimension[i].avl_conv.push_back(dimension[i].SI_unit);
      }

      {
         xmlInitParser();
         std::string path;
	     NgoFileHandler::get()->loadPaths();
         std::string file  = "dimensions_unit.xml";
         xmlDoc* doc = 0L;
         if (NgoFileHandler::get()->isInPath(file.c_str()))
         {
            path = NgoFileHandler::get()->getPath(file.c_str());
            doc = xmlParseFile( path.c_str() );
         }
         else
         {
            std::ostringstream oss;
            oss << "Impossible to find " << file << " in path list.";
            std::string msg = oss.str();
            throw NgoError(msg,"NgoUnit::NgoUnit","numenGo");

         }

         if (doc != NULL)
         {
            xmlNode *root = xmlDocGetRootElement( doc );
            if (root != NULL)
            {
               for (i=0;i<dimension.size();i++)
               {
                  std::string key = "/"+dimension[i].name+"/unit";
                  std::vector<std::string> units_to_add;
                  XmlManagerBase::get()->Read(key,root,&units_to_add);
                  size_t j;
                  for (j=0;j<units_to_add.size();j++)
                  {
                     std::string red_unit = reduce(units_to_add[j].c_str());
                     char * c_red_unit = units_reduce(const_cast<char*> (units_to_add[j].c_str()));
                     red_unit = reduce(units_to_add[j].c_str());
                     if (red_unit == dimension[i].reduced_unit)
                     {
                        dimension[i].avl_conv.push_back(clean(units_to_add[j].c_str()));
                     }
                     else
                     {
   //                     std::cout << "Non homogeneous unit " << units_to_add[j] << " for category " <<  dimension[i].name << "(ref unit is " << dimension[i].SI_unit << ").\n";
                     }
                  }
               }
            }
            xmlFreeDoc(doc);
         }
      }

   }
   catch (NgoError & er)
   {
      er.addScopeError("NgoUnit::NgoUnit");
      er.raise();
   }
   catch (...)
   {
      throw NgoErrorFailedInitialisation("Unknown error loading NgoUnit");
   }
}

NgoUnit::~NgoUnit()
{
   dimension.clear();
// for example to erase special units
}

void NgoUnit::split(
const char * i_str_expr,
std::vector<string> & num,
std::vector<string> & num_exp,
std::vector<string> & den,
std::vector<string> & den_exp
)
{
   std::string str_expr(i_str_expr);
   replace_all(str_expr,"**","^"); // change power symbol ** to ^
   erase_all(str_expr," "); // remove all spaces
   num.clear();
   den.clear();

   std::vector<string> * term;
   std::vector<string> * term_exp;
   string subunit;

   //split clearly identified num and den
   size_t found = str_expr.find_first_of("/*");
   subunit.assign(str_expr,0,found);
   if (!subunit.empty())
      num.push_back(subunit);

   while (found != string::npos)
   {
      subunit.clear();
      if (str_expr[found]=='*') {term = &num;}
      else                        {term = &den;}
      subunit.append(str_expr,found+1,str_expr.find_first_of("/*",found+1)-found-1);
      (*term).push_back(subunit);
      found = str_expr.find_first_of("/*",found+1);
   }
   std::vector<string>::iterator it;
   int i;
   // split other num and den terms separated with -, and get exponents
   for (i=0;i<2;i++)
   {
      switch(i)
      {
      case 0:
         term = &num;
         term_exp = &num_exp;
         break;
      case 1:
         term = &den;
         term_exp = &den_exp;
         break;
      }
      size_t j;
      for (j=0;j<(*term).size();j++)
      {
         found = (*term)[j].find_first_of("-");
         if (found != string::npos)
         {
            //create a subunit with identified unit component
            subunit.assign((*term)[j],0,found);
            // erase subunit created from concerned element */
            (*term)[j].erase(0,found+1);
            //insert subunit at current poisition
            (*term).insert((*term).begin()+j,subunit);
         }
         // check if has an exponent
         (*term_exp).resize((*term).size());
         found = (*term)[j].find_first_of("^");
         if (found != string::npos)
         {
            //add the exponent value with identified unit component
            (*term_exp)[j].assign((*term)[j],found+1,string::npos);
            // erase subunit created from concerned element */
            (*term)[j].erase(found,string::npos);
         }
      }
   }
}

std::string NgoUnit::smartPrint(
std::vector<string> num,
std::vector<string> num_exp,
std::vector<string> den,
std::vector<string> den_exp
)
{

   std::vector<string> *num1, *den1, *num1_exp, *den1_exp;
   int k;

   // regroup same units in numerator and denominator
   for (k=0;k<2;k++)
   {
      switch (k)
      {
      case 0:
         num1     = &num;
         num1_exp = &num_exp;
         break;
      case 1:
         num1     = &den;
         num1_exp = &den_exp;
         break;
      }
      size_t i;
      for (i=0;i<(*num1).size();i++)
      {
         size_t j;
         for (j=i+1;j<(*num1).size();j++)
         {
            if ((*num1)[i]==(*num1)[j])
            {
               // add exponents of a same unit found in numerator
               int res_exp = max(atoi((*num1_exp)[i].c_str()),1) + max(1,atoi((*num1_exp)[j].c_str()));
               std::ostringstream out;
               out << res_exp;
               string str_res_exp = out.str();
               (*num1_exp)[i] = str_res_exp;
               // erase j-th element, but decrease i because size has been reduced
               (*num1).erase((*num1).begin()+j); (*num1_exp).erase((*num1_exp).begin()+j);
            }
         }
      }
   }
   // simplify denominator and numerator same units
   for (k=0;k<2;k++)
   {
      switch (k)
      {
      case 0:
         num1     = &num;
         num1_exp = &num_exp;
         den1     = &den;
         den1_exp = &den_exp;
         break;
      case 1:
         num1     = &den;
         num1_exp = &den_exp;
         den1     = &num;
         den1_exp = &num_exp;
         break;
      }
      size_t i;
      for (i=0;i<(*num1).size();i++)
      {
         size_t j;
         for (j=0;j<(*den1).size();j++)
         {
            if ((*num1)[i]==(*den1)[j])
            {
               int res_exp = max(atoi((*num1_exp)[i].c_str()),1) - max(atoi((*den1_exp)[j].c_str()),1);
               std::ostringstream out;
               out << abs(res_exp);
               string str_res_exp = out.str();
               if (res_exp==0) // same exponent, just simplify
               {
                  (*num1).erase((*num1).begin()+i); (*num1_exp).erase((*num1_exp).begin()+i);
                  (*den1).erase((*den1).begin()+j); (*den1_exp).erase((*den1_exp).begin()+j);
               }
               else if (res_exp>0) // no more denominator
               {
                  (*num1_exp)[i] = str_res_exp;
                  (*den1).erase((*den1).begin()+j); (*den1_exp).erase((*den1_exp).begin()+j);
               }
               else // no more denominator
               {
                  (*den1_exp)[j] = str_res_exp;
                  (*num1).erase((*num1).begin()+i); (*num1_exp).erase((*num1_exp).begin()+i);
               }
            }
         }
      }
   }

   std::string o_str_expr;

   if (num.empty()&&den.empty())
      return o_str_expr;

   for (k=0;k<2;k++)
   {
      switch (k)
      {
      case 0:
         num1     = &num;
         num1_exp = &num_exp;
         break;
      case 1:
         num1     = &den;
         num1_exp = &den_exp;
         break;
      }
      int i;
      for (i=0;i<(*num1).size();i++)
      {
         if (!(*num1_exp)[i].empty())
            (*num1)[i] = (*num1)[i] + "^" + (*num1_exp)[i]; // concatenate exponent
      }
      for (i=(*num1).size()-1;i>0;i--)
      {
         (*num1)[i-1] += " - " + (*num1)[i];
         (*num1).erase((*num1).begin()+i);
      }
   }
   if (num.empty())
   {
      num.push_back("1");
   }
   if (!den.empty())
   {
      den[0] = " / " + den[0];
   }
   else
   {
      den.push_back("");
   }
   o_str_expr = num[0] + den[0];
   if ((o_str_expr=="null")||(o_str_expr=="NULL"))
      o_str_expr.clear();
   return o_str_expr;
}

std::string NgoUnit::clean(const char * i_str_expr)
{
   std::vector<std::string> num , num_exp, den, den_exp;
   split(i_str_expr,num,num_exp,den,den_exp);
   std::string ret = smartPrint(num,num_exp,den,den_exp);
   return ret;
}

std::vector<std::string> NgoUnit::listUnits(
const char * i_str_expr
)
{
   std::vector<string> unit_list;
   std::string red_unit(reduce(clean(i_str_expr).c_str()));
   std::string cleaned_unit = clean(i_str_expr);

   unsigned i;
   for (i=0;i<dimension.size();i++)
   {
      if (dimension[i].reduced_unit == red_unit)
      {
         unit_list.push_back(dimension[i].default_unit);
         size_t j;
         bool found = false;
         for (unsigned j=0;j<dimension[i].avl_conv.size();j++)
         {
            if (dimension[i].avl_conv[j] == cleaned_unit)
                  found = true;
         }

         if (found)
            unit_list.insert(unit_list.begin(),dimension[i].avl_conv.begin(),dimension[i].avl_conv.end());
         else
            unit_list.insert(unit_list.end(),dimension[i].avl_conv.begin(),dimension[i].avl_conv.end());
      }
   }
   // remove duplicates
   for (i=0;i<unit_list.size();i++)
   {
      size_t j;
      for (j=i+1;j<unit_list.size();j++)
      {
         if (unit_list[i]==unit_list[j])
         {
            unit_list.erase(unit_list.begin()+j);
            j--;
         }
      }
   }
   return unit_list;
}


std::string NgoUnit::multiply(
const char * i_str_expr1,
const char * i_str_expr2
)
{
   std::vector<std::string> num1 , num_exp1, den1, den_exp1;
   std::vector<std::string> num2 , num_exp2, den2, den_exp2;
   split(i_str_expr1,num1,num_exp1,den1,den_exp1);
   split(i_str_expr2,num2,num_exp2,den2,den_exp2);
   num1.insert(num1.end(),num2.begin(),num2.end());
   den1.insert(den1.end(),den2.begin(),den2.end());
   num_exp1.insert(num_exp1.end(),num_exp2.begin(),num_exp2.end());
   den_exp1.insert(den_exp1.end(),den_exp2.begin(),den_exp2.end());
   return smartPrint(num1,num_exp1,den1,den_exp1);

}

std::string NgoUnit::inverse(const char * i_str_expr)
{
   std::vector<std::string> num1, num_exp1, den1, den_exp1;
   split(i_str_expr,num1,num_exp1,den1,den_exp1);
   return smartPrint(den1,den_exp1,num1,num_exp1);
}

void NgoUnit::conversion(
const char * i_str_from_unit,
const char * i_str_to_unit,
double & o_conv_factor,
double & o_conv_offset
)
{
   string str_from_unit(clean(i_str_from_unit));
   string str_to_unit(clean(i_str_to_unit));

   char s1[256]; strcpy(s1,str_from_unit.c_str());
   char s2[256]; strcpy(s2,str_to_unit.c_str());
   /* conversion to SI : need to find which category fits s1 */
   if (str_to_unit.empty())
   {
      string red_unit(reduce(i_str_from_unit));
      replace_all(red_unit,"gram","kilogram");
      strcpy(s2,red_unit.c_str());
   }

   o_conv_factor = units_convert(s1,s2);
   if (units_check_exception())
   {
      string msg(units_check_exception());
      units_clear_exception();
      throw NgoErrorInvalidArgument(3,msg,"Unit::convert");
   }

   o_conv_offset = 0.;
   if (reduce(str_from_unit.c_str())=="kelvin")
   {
      double offset[2];
      string temp;
      int i;
      for (i=0;i<2;i++)
      {
         offset[i] = 0.;
         switch (i)
         {
         case 0:
            temp = str_from_unit;
            break;
         case 1:
            temp = str_to_unit;
            break;
         }
         if (temp == "degC")
            offset[i] = 273.15;
         else if (temp == "degF")
            offset[i] = 459.67 * 5. / 9.;
      }
      o_conv_offset = (offset[0] - offset[1]) * units_convert("kelvin",s2);
   }

}

double NgoUnit::convert(
const double i_value,
const char * i_str_from_unit,
const char * i_str_to_unit,
const bool is_diff
)
{
   double fact_conv, off_conv;
   conversion(i_str_from_unit,i_str_to_unit,fact_conv,off_conv);

   return (i_value * fact_conv + (!is_diff) * off_conv);
}

std::vector<double> NgoUnit::convert(
const std::vector<double> & i_value,
const char * i_str_from_unit,
const char * i_str_to_unit,
const bool is_diff
)
{
   double fact_conv, off_conv;
   conversion(i_str_from_unit,i_str_to_unit,fact_conv,off_conv);

   std::vector<double> values(i_value.size());
   size_t i;

   for (i=0;i<i_value.size();i++)
   {
      values[i] = i_value[i] * fact_conv + (!is_diff) * off_conv;
   }

   return values;
}

void NgoUnit::setConversion(
const char * i_str_from_unit,
const char * i_str_to_unit,
const double i_conv_factor,
const double i_conv_offset
)
{
// TODO (Administrator#1#): Handle customized offsets
   try
   {
      std::ostringstream oss;
      oss << i_conv_factor << i_str_to_unit;
      string new_unit = oss.str();
      units_new(const_cast<char*> (clean(i_str_from_unit).c_str())
               ,const_cast<char*> (new_unit.c_str()));
      if (units_check_exception())
      {
         string msg(units_check_exception());
         units_clear_exception();
         throw NgoErrorInvalidArgument(3,msg,"setConversion::convert");
      }
   }
   catch(...)
   {
      throw NgoErrorUnknown("Unknown error in NgoUnit::setConversion");
   }
}
