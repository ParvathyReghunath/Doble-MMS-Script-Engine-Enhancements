/*  This file created from input file '..\..\mvl\usr\iecgoose\uca_obj.odf'
    Leaf Access Parameter (LAP) File: '..\..\mvl\usr\iecgoose\leafmap.xml'
	Created Mon May 27 21:13:49 2019
*/

#ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED
#define FOUNDRY_OUTPUT_HEADER_INCLUDED

#include "mvl_uca.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* MVL Type Control Information */
extern MVL_TYPE_CTRL *mvl_type_ctrl;
extern ST_INT mvl_num_types;

/************************************************************************/


/* MVL Type ID's */


/************************************************************************/

/************************************************************************/


/*	Common Strings Index Defines	*/

#ifndef USE_RT_TYPE_2

#endif


/************************************************************************/
/* TYPEDEFS for MMS TYPES	                                              */
/************************************************************************/

/* Use ":CF", "-notypedefs" in the ODF file to not include this line */
#define USE_MMS_TYPEDEFS

#if defined (USE_MMS_TYPEDEFS)

/************************************************************************/
#endif /* if defined (USE_MMS_TYPEDEFS) */
/************************************************************************/

/************************************************************************/
/* UCA Runtime Type Function Indexes                                    */
/* These are generated by Foundry and should not be changed             */

#define MVLU_NULL_RD_IND_INDEX	0


#define MVLU_NULL_WR_IND_INDEX	0

/************************************************************************/
/* UCA Read/Write Indication function external declarations */
/* These functions must be provided externally              */

ST_VOID mvlu_null_rd_ind (MVLU_RD_VA_CTRL *mvluRdVaCtrl);

ST_VOID mvlu_null_wr_ind (MVLU_WR_VA_CTRL *mvluWrVaCtrl);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FOUNDRY_OUTPUT_HEADER_INCLUDED */