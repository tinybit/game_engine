//***************************************************************************************
//
// File supervisor: Softimage 3D Games & 3D Bridge team
//
// (c) Copyright 2001-2002 Avid Technology, Inc. . All rights reserved.
//
//***************************************************************************************

/****************************************************************************************
THIS CODE IS PUBLISHED AS A SAMPLE ONLY AND IS PROVIDED "AS IS".
IN NO EVENT SHALL SOFTIMAGE, AVID TECHNOLOGY, INC. AND/OR THEIR RESPECTIVE
SUPPLIERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS CODE .

COPYRIGHT NOTICE. Copyright ? 1999-2002 Avid Technology Inc. . All rights reserved. 
 
SOFTIMAGE is a registered trademark of Avid Technology Inc. or its subsidiaries 
or divisions. Windows NT is a registered trademark of Microsoft Corp. All other
trademarks contained herein are the property of their respective owners. 
****************************************************************************************/
#ifndef _DIRECTIONALLIGHT_H
#define _DIRECTIONALLIGHT_H

#include "Light.h"

//! Describes a point light.
class XSIEXPORT CSLDirectionalLight
    : public CSLLight
{
public:
	/*! Constructor
	* \param in_pScene scene to add this point light to
	* \param in_pModel model to add this light to 
	* \param in_pTemplate IO layer template to connect to
	* \return an instance of CSLPointLight
	*/    
    CSLDirectionalLight(CSLScene* in_pScene, CSLModel *in_pModel, CdotXSITemplate* in_pTemplate);

    /*! Destructor
	*/
	virtual ~CSLDirectionalLight();

	/*! Return the direction of the light.
		\return CSIBCVector3D Light direction.
	*/
	CSIBCVector3D GetDirection();

	/*! Change the direction of the light.
		\param	in_rValue	New light direction.
		\return SI_Void
	*/
	SI_Void SetDirection(CSIBCVector3D &in_rValue);
    
	/*! Synchronizes this directional light instance with its IO layer representation
	* \return SI_Error
	* \retval SI_SUCCESS this function always returns SI_SUCCESS
	*/
    virtual SI_Error Synchronize();

	/*! Get the template type
	* \return ETemplateType the template type of the directional light
	*/
    virtual ETemplateType Type();

	/*! Gets a handle on one of this light's parameters by using its name
	* \param in_szName the name of the parameter to get the handle from
	* \return CSLAnimatableType* the handle to the parameter
	*/
    virtual CSLAnimatableType* ParameterFromName(SI_Char *in_szName);

	/*! Gets a handle on one of this light's parameters by using its name and type
	* \param in_szName the name of the parameter to get the handle from
	* \param in_Type the type of the parameter to get the handle from
	* \return CSLAnimatableType* the handle to the parameter
	*/    
	virtual CSLAnimatableType* ParameterFromType(EFCurveType in_Type, SI_Char *in_szParameterName);

protected:
	virtual SI_Bool ConstraintTypeIsValid(CSLConstraint::EConstraintType in_ConstraintType);

private:
    CSLVector3DProxy m_Direction;

	void *m_pReserved;	// reserved for future extension
};

#endif
