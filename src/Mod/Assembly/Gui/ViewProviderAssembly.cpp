/***************************************************************************
 *   Copyright (c) 2011 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#ifndef _PreComp_
# include <Inventor/nodes/SoGroup.h>
#endif

#include "ViewProviderAssembly.h"
#include <Gui/Command.h>
#include <Gui/Document.h>

#include <Mod/Assembly/App/ItemAssembly.h>

using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderItemAssembly,AssemblyGui::ViewProviderItem)

ViewProviderItemAssembly::ViewProviderItemAssembly()
{
}

ViewProviderItemAssembly::~ViewProviderItemAssembly()
{
}

bool ViewProviderItemAssembly::doubleClicked(void)
{
    Gui::Command::assureWorkbench("AssemblyWorkbench");
    Gui::Command::doCommand(Gui::Command::Doc,"AssemblyGui.setActiveAssembly(App.activeDocument().%s)",this->getObject()->getNameInDocument());
    return true;
}

void ViewProviderItemAssembly::attach(App::DocumentObject *pcFeat)
{
    // call parent attach method
    ViewProviderGeometryObject::attach(pcFeat);


    // putting all together with the switch
    addDisplayMaskMode(getChildRoot(), "Main");
}

void ViewProviderItemAssembly::setDisplayMode(const char* ModeName)
{
    if ( strcmp("Main",ModeName)==0 )
        setDisplayMaskMode("Main");

    ViewProviderGeometryObject::setDisplayMode( ModeName );
}

std::vector<std::string> ViewProviderItemAssembly::getDisplayModes(void) const
{
    // get the modes of the father
    std::vector<std::string> StrList = ViewProviderGeometryObject::getDisplayModes();

    // add your own modes
    StrList.push_back("Main");

    return StrList;
}


std::vector<App::DocumentObject*> ViewProviderItemAssembly::claimChildren(void)const
{

    return static_cast<Assembly::ItemAssembly*>(getObject())->Items.getValues();

}

std::vector<App::DocumentObject*> ViewProviderItemAssembly::claimChildren3D(void)const
{

    return static_cast<Assembly::ItemAssembly*>(getObject())->Items.getValues();

}
