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
# include <Precision.hxx>
#endif

#include <Mod/PartDesign/App/FeaturePocket.h>

#include "ui_TaskPadPocketParameters.h"
#include "TaskPocketParameters.h"


using namespace PartDesignGui;
using namespace Gui;

/* TRANSLATOR PartDesignGui::TaskPocketParameters */

TaskPocketParameters::TaskPocketParameters(ViewProviderPocket *PocketView,QWidget *parent, bool newObj)
    : TaskExtrudeParameters(PocketView, parent, "PartDesign_Pocket", tr("Pocket parameters"))
    , oldLength(0)
{
    setupDialog(newObj, "User parameter:BaseApp/History/Pocket/");
}

TaskPocketParameters::~TaskPocketParameters()
{
}

void TaskPocketParameters::translateModeList(int index)
{
    ui->changeMode->clear();
    ui->changeMode->addItem(tr("Dimension"));
    ui->changeMode->addItem(tr("Through all"));
    ui->changeMode->addItem(tr("To first"));
    ui->changeMode->addItem(tr("Up to face"));
    ui->changeMode->addItem(tr("Two dimensions"));
    ui->changeMode->setCurrentIndex(index);
}

void TaskPocketParameters::translateTooltips()
{
    ui->offsetEdit->setToolTip(tr("Offset from face at which pocket will end"));
    ui->checkBoxReversed->setToolTip(tr("Reverses pocket direction"));
}

void TaskPocketParameters::onModeChanged(int index)
{
    PartDesign::Pocket* pcPocket = static_cast<PartDesign::Pocket*>(vp->getObject());

    switch (static_cast<Modes>(index)) {
        case Modes::Dimension:
            // Why? See below for "UpToFace"
            if (oldLength < Precision::Confusion())
                oldLength = 5.0;
            pcPocket->Length.setValue(oldLength);
            ui->lengthEdit->setValue(oldLength);
            pcPocket->Type.setValue("Length");
            break;
        case Modes::ThroughAll:
            oldLength = pcPocket->Length.getValue();
            pcPocket->Type.setValue("ThroughAll");
            break;
        case Modes::ToFirst:
            oldLength = pcPocket->Length.getValue();
            pcPocket->Type.setValue("UpToFirst");
            break;
        case Modes::ToFace: {
            // Note: ui->checkBoxReversed is purposely enabled because the selected face
            // could be a circular one around the sketch
            // Also note: Because of the code at the beginning of Pocket::execute() which is used
            // to detect broken legacy parts, we must set the length to zero here!
            oldLength = pcPocket->Length.getValue();
            pcPocket->Type.setValue("UpToFace");
            QSignalBlocker blocker(ui->lengthEdit);
            pcPocket->Length.setValue(0.0);
            ui->lengthEdit->setValue(0.0);
            if (!pcPocket->UpToFace.getValue()) {
                setCheckboxes();
                return;
            }
            break;
        }
        case Modes::TwoDimensions:
            oldLength = pcPocket->Length.getValue();
            pcPocket->Type.setValue("TwoLengths");
            break;
    }

    setCheckboxes();
    recomputeFeature();
}

void TaskPocketParameters::apply()
{
    QString facename = QStringLiteral("None");
    if (static_cast<Modes>(getMode()) == Modes::ToFace) {
        facename = getFaceName();
    }
    applyParameters(facename);
}

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgPocketParameters::TaskDlgPocketParameters(ViewProviderPocket *PocketView)
    : TaskDlgSketchBasedParameters(PocketView)
{
    assert(vp);
    Content.push_back ( new TaskPocketParameters(PocketView ) );
}

#include "moc_TaskPocketParameters.cpp"
