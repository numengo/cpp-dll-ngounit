#ifndef _NgoUnitExports_h
#define _NgoUnitExports_h
/*******************************************************************************
   FILE DESCRIPTION
*******************************************************************************/
/*!
@file NgoUnitExports.h
@author Cedric ROMAN - roman@numengo.com
@date February 2009
@brief File containing defines for exports in case of dynamic libraries
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

/*******************************************************************************
   DEFINES / TYPDEFS / ENUMS
*******************************************************************************/
#ifdef NGO_UNIT_USE_DYN
   #ifdef  _MSC_VER
   	#ifndef NGO_UNIT_EXPORT
   		#ifdef NGO_UNIT_MAKE_DLL
   			#define NGO_UNIT_EXPORT __declspec( dllexport )
   		#else
   			#define NGO_UNIT_EXPORT __declspec( dllimport )
   		#endif
   	#endif
   	#pragma warning( disable: 4251 )
   	#pragma warning( disable: 4275 )
   	#pragma warning( disable: 4996 )
   	#pragma warning( disable: 4661 )
   #else
   	#ifndef NGO_UNIT_EXPORT
   		#ifdef NGO_UNIT_MAKE_DLL
   			#define NGO_UNIT_EXPORT
   		#else
   			#define NGO_UNIT_EXPORT
   		#endif
   	#endif
   #endif
#else
	#define NGO_UNIT_EXPORT
#endif


#endif
