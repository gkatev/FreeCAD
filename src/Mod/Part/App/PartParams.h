/****************************************************************************
 *   Copyright (c) 2022 Zheng Lei (realthunder) <realthunder.dev@gmail.com> *
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#ifndef PART_APP_PARAMS_H
#define PART_APP_PARAMS_H

#include <Mod/Part/PartGlobal.h>

/*[[[cog
import PartParams
PartParams.declare()
]]]*/

// Auto generated code (Tools/params_utils.py:82)
#include <Base/Parameter.h>


// Auto generated code (Tools/params_utils.py:90)
namespace Part {
/** Convenient class to obtain Part/PartDesign related parameters

 * The parameters are under group "User parameter:BaseApp/Preferences/Mod/Part"
 *
 * This class is auto generated by Mod/Part/App/PartParams.py. Modify that file
 * instead of this one, if you want to add any parameter. You need
 * to install Cog Python package for code generation:
 * @code
 *     pip install cogapp
 * @endcode
 *
 * Once modified, you can regenerate the header and the source file,
 * @code
 *     python3 -m cogapp -r Mod/Part/App/PartParams.h Mod/Part/App/PartParams.cpp
 * @endcode
 *
 * You can add a new parameter by adding lines in Mod/Part/App/PartParams.py. Available
 * parameter types are 'Int, UInt, String, Bool, Float'. For example, to add
 * a new Int type parameter,
 * @code
 *     ParamInt(parameter_name, default_value, documentation, on_change=False)
 * @endcode
 *
 * If there is special handling on parameter change, pass in on_change=True.
 * And you need to provide a function implementation in Mod/Part/App/PartParams.cpp with
 * the following signature.
 * @code
 *     void PartParams:on<parameter_name>Changed()
 * @endcode
 */
class PartExport PartParams {
public:
    static ParameterGrp::handle getHandle();

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter ShapePropertyCopy
    static const bool & getShapePropertyCopy();
    static const bool & defaultShapePropertyCopy();
    static void removeShapePropertyCopy();
    static void setShapePropertyCopy(const bool &v);
    static const char *docShapePropertyCopy();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter DisableShapeCache
    static const bool & getDisableShapeCache();
    static const bool & defaultDisableShapeCache();
    static void removeDisableShapeCache();
    static void setDisableShapeCache(const bool &v);
    static const char *docDisableShapeCache();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter CommandOverride
    static const long & getCommandOverride();
    static const long & defaultCommandOverride();
    static void removeCommandOverride();
    static void setCommandOverride(const long &v);
    static const char *docCommandOverride();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter EnableWrapFeature
    static const long & getEnableWrapFeature();
    static const long & defaultEnableWrapFeature();
    static void removeEnableWrapFeature();
    static void setEnableWrapFeature(const long &v);
    static const char *docEnableWrapFeature();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter CopySubShape
    static const bool & getCopySubShape();
    static const bool & defaultCopySubShape();
    static void removeCopySubShape();
    static void setCopySubShape(const bool &v);
    static const char *docCopySubShape();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter UseBrepToolsOuterWire
    static const bool & getUseBrepToolsOuterWire();
    static const bool & defaultUseBrepToolsOuterWire();
    static void removeUseBrepToolsOuterWire();
    static void setUseBrepToolsOuterWire(const bool &v);
    static const char *docUseBrepToolsOuterWire();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter UseBaseObjectName
    static const bool & getUseBaseObjectName();
    static const bool & defaultUseBaseObjectName();
    static void removeUseBaseObjectName();
    static void setUseBaseObjectName(const bool &v);
    static const char *docUseBaseObjectName();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter AutoGroupSolids
    static const bool & getAutoGroupSolids();
    static const bool & defaultAutoGroupSolids();
    static void removeAutoGroupSolids();
    static void setAutoGroupSolids(const bool &v);
    static const char *docAutoGroupSolids();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter SingleSolid
    static const bool & getSingleSolid();
    static const bool & defaultSingleSolid();
    static void removeSingleSolid();
    static void setSingleSolid(const bool &v);
    static const char *docSingleSolid();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter UsePipeForExtrusionDraft
    static const bool & getUsePipeForExtrusionDraft();
    static const bool & defaultUsePipeForExtrusionDraft();
    static void removeUsePipeForExtrusionDraft();
    static void setUsePipeForExtrusionDraft(const bool &v);
    static const char *docUsePipeForExtrusionDraft();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter LinearizeExtrusionDraft
    static const bool & getLinearizeExtrusionDraft();
    static const bool & defaultLinearizeExtrusionDraft();
    static void removeLinearizeExtrusionDraft();
    static void setLinearizeExtrusionDraft(const bool &v);
    static const char *docLinearizeExtrusionDraft();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter AutoCorrectLink
    static const bool & getAutoCorrectLink();
    static const bool & defaultAutoCorrectLink();
    static void removeAutoCorrectLink();
    static void setAutoCorrectLink(const bool &v);
    static const char *docAutoCorrectLink();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter RefineModel
    static const bool & getRefineModel();
    static const bool & defaultRefineModel();
    static void removeRefineModel();
    static void setRefineModel(const bool &v);
    static const char *docRefineModel();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter AuxGroupUniqueLabel
    static const bool & getAuxGroupUniqueLabel();
    static const bool & defaultAuxGroupUniqueLabel();
    static void removeAuxGroupUniqueLabel();
    static void setAuxGroupUniqueLabel(const bool &v);
    static const char *docAuxGroupUniqueLabel();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter SplitEllipsoid
    static const bool & getSplitEllipsoid();
    static const bool & defaultSplitEllipsoid();
    static void removeSplitEllipsoid();
    static void setSplitEllipsoid(const bool &v);
    static const char *docSplitEllipsoid();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter ParallelRunThreshold
    static const long & getParallelRunThreshold();
    static const long & defaultParallelRunThreshold();
    static void removeParallelRunThreshold();
    static void setParallelRunThreshold(const long &v);
    static const char *docParallelRunThreshold();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter AutoValidateShape
    static const bool & getAutoValidateShape();
    static const bool & defaultAutoValidateShape();
    static void removeAutoValidateShape();
    static void setAutoValidateShape(const bool &v);
    static const char *docAutoValidateShape();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter FixShape
    static const bool & getFixShape();
    static const bool & defaultFixShape();
    static void removeFixShape();
    static void setFixShape(const bool &v);
    static const char *docFixShape();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter LoftMaxDegree
    static const unsigned long & getLoftMaxDegree();
    static const unsigned long & defaultLoftMaxDegree();
    static void removeLoftMaxDegree();
    static void setLoftMaxDegree(const unsigned long &v);
    static const char *docLoftMaxDegree();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter MinimumDeviation
    static const double & getMinimumDeviation();
    static const double & defaultMinimumDeviation();
    static void removeMinimumDeviation();
    static void setMinimumDeviation(const double &v);
    static const char *docMinimumDeviation();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter MeshDeviation
    static const double & getMeshDeviation();
    static const double & defaultMeshDeviation();
    static void removeMeshDeviation();
    static void setMeshDeviation(const double &v);
    static const char *docMeshDeviation();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter MeshAngularDeflection
    static const double & getMeshAngularDeflection();
    static const double & defaultMeshAngularDeflection();
    static void removeMeshAngularDeflection();
    static void setMeshAngularDeflection(const double &v);
    static const char *docMeshAngularDeflection();
    //@}

    // Auto generated code (Tools/params_utils.py:139)
    //@{
    /// Accessor for parameter MinimumAngularDeflection
    static const double & getMinimumAngularDeflection();
    static const double & defaultMinimumAngularDeflection();
    static void removeMinimumAngularDeflection();
    static void setMinimumAngularDeflection(const double &v);
    static const char *docMinimumAngularDeflection();
    //@}

// Auto generated code (Tools/params_utils.py:179)
}; // class PartParams
} // namespace Part
//[[[end]]]

#endif // PART_APP_PARAMS_H
