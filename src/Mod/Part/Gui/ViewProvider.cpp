/***************************************************************************
 *   Copyright (c) 2004 Juergen Riegel <juergen.riegel@web.de>             *
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
# include <QAction>
# include <QMenu>
# include <QMouseEvent>
# include <QPixmap>
#endif

#include <Base/Console.h>
#include <Base/Exception.h>
#include <App/Document.h>
#include <App/AutoTransaction.h>
#include <Gui/Command.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Selection.h>
#include <Gui/Action.h>
#include <Gui/Command.h>

#include <Mod/Part/App/AttachExtension.h>
#include <Mod/Part/App/PartFeature.h>

#include "ViewProvider.h"


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderPart, PartGui::ViewProviderPartExt)


ViewProviderPart::ViewProviderPart()
{
}

ViewProviderPart::~ViewProviderPart()
{
}

bool ViewProviderPart::doubleClicked(void)
{
    try {
        QString text = QObject::tr("Edit %1").arg(QString::fromUtf8(getObject()->Label.getValue()));
        Gui::Command::openCommand(text.toUtf8());
        FCMD_SET_EDIT(pcObject);
        return true;
    }
    catch (const Base::Exception& e) {
        Base::Console().Error("%s\n", e.what());
        return false;
    }
}

QPixmap ViewProviderPart::getTagIcon() const
{
    return QPixmap();
}

void ViewProviderPart::applyColor(const Part::ShapeHistory& hist,
                                  const std::vector<App::Color>& colBase,
                                  std::vector<App::Color>& colBool)
{
    std::map<int, std::vector<int> >::const_iterator jt;
    // apply color from modified faces
    for (jt = hist.shapeMap.begin(); jt != hist.shapeMap.end(); ++jt) {
        std::vector<int>::const_iterator kt;
        for (kt = jt->second.begin(); kt != jt->second.end(); ++kt) {
            colBool[*kt] = colBase[jt->first];
        }
    }
}

void ViewProviderPart::applyTransparency(const float& transparency,
                                  std::vector<App::Color>& colors)
{
    if (transparency != 0.0) {
        // transparency has been set object-wide
        std::vector<App::Color>::iterator j;
        for (j = colors.begin(); j != colors.end(); ++j) {
            // transparency hasn't been set for this face
            if (j->a == 0.0)
                j->a = transparency/100.0; // transparency comes in percent
        }
    }
}

static inline App::PropertyLinkList *getShapesLinks(App::DocumentObject *obj)
{
    auto feat = Base::freecad_dynamic_cast<Part::Feature>(obj);
    if (!feat)
        return nullptr;
    return feat->getShapeLinksProperty();
}

bool ViewProviderPart::canReplaceObject(App::DocumentObject *oldObj, App::DocumentObject *newObj)
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentReplacementProperty()) {
            if (prop->getValue() == oldObj)
                return true;
        }
        if (auto prop = feat->getShapeLinksProperty())
            return oldObj && newObj
                        && prop->find(oldObj->getNameInDocument())
                        && !prop->find(newObj->getNameInDocument());
    }
    return false;
}

bool ViewProviderPart::reorderObjects(const std::vector<App::DocumentObject *> &objs, App::DocumentObject *before)
{
    return Gui::ViewProvider::reorderObjectsInProperty(getShapesLinks(getObject()), objs, before);
}

bool ViewProviderPart::canReorderObject(App::DocumentObject *obj, App::DocumentObject *before)
{
    return Gui::ViewProvider::canReorderObjectInProperty(getShapesLinks(getObject()), obj, before);
}


bool ViewProviderPart::canDropObjects() const
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject()))
        return feat->get_ShapeContentOwnerProperty() != nullptr;
    return inherited::canDropObjects();
}

bool ViewProviderPart::canDropObject(App::DocumentObject *obj) const
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject()))
        return feat->get_ShapeContentOwnerProperty() != nullptr;
    return inherited::canDropObject(obj);
}

void ViewProviderPart::dropObject(App::DocumentObject *obj)
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (feat->get_ShapeContentOwnerProperty() != nullptr) {
            auto prop = feat->getShapeContentReplacementProperty(true);
            prop->setValue(obj);
            obj->Visibility.setValue(false);
            return;
        }
    }
    return inherited::dropObject(obj);
}

bool ViewProviderPart::canDragObjects() const
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentReplacementProperty())
            return prop->getValue() != nullptr;
    }
    return inherited::canDragObjects();
}

bool ViewProviderPart::canDragObject(App::DocumentObject *obj) const
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentReplacementProperty())
            return obj && prop->getValue() == obj;
    }
    return inherited::canDropObject(obj);
}

void ViewProviderPart::dragObject(App::DocumentObject *obj)
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentReplacementProperty()) {
            if (prop->getValue() == obj) {
                prop->setValue(nullptr);
                return;
            }
        }
    }
    return inherited::dropObject(obj);
}

void ViewProviderPart::updateData(const App::Property *prop)
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (prop == feat->getShapeContentSuppressedProperty()
                || prop == feat->getShapeContentReplacementProperty()
                || prop == feat->getShapeContentReplacementSuppressedProperty())
            signalChangeIcon();
    }
    inherited::updateData(prop);
}

static QByteArray _SuppressedTag("Part::Suppressed");
static QByteArray _ReplacementTag("Part::Replacement");

void ViewProviderPart::getExtraIcons(std::vector<std::pair<QByteArray, QPixmap> > &icons) const
{
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentSuppressedProperty()) {
            if (prop->getValue())
                icons.emplace_back(_SuppressedTag, Gui::BitmapFactory().pixmap("Part_Suppressed.svg"));
        }
        if (auto prop = feat->getShapeContentReplacementProperty()) {
            if (prop->getValue()) {
                auto suppressed = feat->getShapeContentReplacementSuppressedProperty();
                icons.emplace_back(_ReplacementTag, Gui::BitmapFactory().pixmap(
                            suppressed && suppressed->getValue() ? "Part_ReplacementSuppressed" : "Part_Replacement"));
            }
        }
    }
    inherited::getExtraIcons(icons);
}

QString ViewProviderPart::getToolTip(const QByteArray &iconTag) const
{
    if (iconTag == _SuppressedTag)
        return QObject::tr("Shape content suppressed. Alt + Click this icon to enable");
    else if (iconTag == _ReplacementTag)
        return QObject::tr("Shape content replacement. Alt + Click this icon to toggle the replacement");
    return inherited::getToolTip(iconTag);
}

bool ViewProviderPart::iconMouseEvent(QMouseEvent *ev, const QByteArray &iconTag)
{
    if (ev->type() == QEvent::MouseButtonPress) {
        if (iconTag == _SuppressedTag) {
            if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
                if (auto prop = feat->getShapeContentSuppressedProperty()) {
                    try {
                        static const char *title = QT_TR_NOOP("Toggle shape suppress");
                        App::AutoTransaction guard(title);
                        prop->setValue(!prop->getValue());
                        Gui::Command::updateActive();
                    } catch (Base::Exception &e) {
                        e.ReportException();
                    }
                }
            }
            return true;
        }
        else if (iconTag == _ReplacementTag) {
            if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
                if (auto prop = feat->getShapeContentReplacementProperty()) {
                    if (prop->getValue()) {
                        try {
                            static const char *title = QT_TR_NOOP("Toggle shape replacement");
                            App::AutoTransaction guard(title);
                            auto suppressed = feat->getShapeContentReplacementSuppressedProperty(true);
                            suppressed->setValue(!suppressed->getValue());
                            Gui::Command::updateActive();
                        } catch (Base::Exception &e) {
                            e.ReportException();
                        }
                    }
                }
            }
            return true;
        }
    }
    return inherited::iconMouseEvent(ev, iconTag);
}

std::vector<App::DocumentObject*> ViewProviderPart::claimChildren(void) const
{
    auto res = inherited::claimChildren();
    if (auto feat = Base::freecad_dynamic_cast<Part::Feature>(getObject())) {
        if (auto prop = feat->getShapeContentReplacementProperty()) {
            if (prop->getValue())
                res.push_back(prop->getValue());
        }
        if (auto prop = feat->getShapeContentsProperty()) {
            for (auto obj : prop->getValues())
                res.push_back(obj);
        }
    }
    return res;
}

template<class F>
static inline void foreachFeature(Part::Feature *mainFeature, F func)
{
    for (const auto &selT : Gui::Selection().getSelectionT()) {
        if (auto obj = selT.getObject()) {
            if (obj->getTypeId() == Part::Feature::getClassTypeId()) {
                if (obj == mainFeature)
                    mainFeature = nullptr;
                func(static_cast<Part::Feature*>(obj));
            }
        }
    }
    if (mainFeature)
        func(mainFeature);
}

void ViewProviderPart::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    if (!getObject())
        return;
    if (getObject()->getTypeId() == Part::Feature::getClassTypeId()) {
        // Expose 'Expand shape content' menu action only if it is a Part::Feature
        // and not any of its derived type
        auto feat = static_cast<Part::Feature*>(getObject());
        auto propContents = feat->getShapeContentsProperty();
        auto shapeType = feat->Shape.getShape().shapeType(/*silent*/true);
        if (propContents || (shapeType != TopAbs_SHAPE && shapeType != TopAbs_VERTEX)) {
            static const char *title = QT_TR_NOOP("Expand shape contents");
            menu->addAction(QObject::tr(title), [feat](){
                try {
                    App::AutoTransaction guard(title);
                    foreachFeature(feat, [](Part::Feature *f) {
                        f->getShapeContentsProperty(true);
                        f->expandShapeContents();
                    });
                    Gui::Command::updateActive();
                } catch (Base::Exception &e) {
                    e.ReportException();
                }
            });
        }
        if (propContents && propContents->getSize()>0) {
            static const char *title = QT_TR_NOOP("Collapse shape contents");
            menu->addAction(QObject::tr(title), [feat](){
                try {
                    App::AutoTransaction guard(title);
                    foreachFeature(feat, [](Part::Feature *f) {
                        f->collapseShapeContents(true);
                    });
                    Gui::Command::updateActive();
                } catch (Base::Exception &e) {
                    e.ReportException();
                }
            });
        }
        if (auto prop = feat->get_ShapeContentOwnerProperty()) {
            if (prop->getValue()) {
                static const char *title = QT_TR_NOOP("Toggle shape suppress");
                menu->addAction(QObject::tr(title), [feat](){
                    try {
                        App::AutoTransaction guard(title);
                        foreachFeature(feat, [](Part::Feature *f) {
                            if (f->get_ShapeContentOwnerProperty()) {
                                auto prop = f->getShapeContentSuppressedProperty(true);
                                prop->setValue(!prop->getValue());
                            }
                        });
                        Gui::Command::updateActive();
                    } catch (Base::Exception &e) {
                        e.ReportException();
                    }
                });
            }
        }
        if (auto prop = feat->getShapeContentReplacementProperty()) {
            if (prop->getValue()) {
                static const char *title = QT_TR_NOOP("Toggle shape replacement");
                menu->addAction(QObject::tr(title), [feat](){
                    try {
                        App::AutoTransaction guard(title);
                        foreachFeature(feat, [](Part::Feature *f) {
                            auto prop = f->getShapeContentReplacementProperty();
                            if (prop && prop->getValue()) {
                                auto prop = f->getShapeContentReplacementSuppressedProperty(true);
                                prop->setValue(!prop->getValue());
                            }
                        });
                        Gui::Command::updateActive();
                    } catch (Base::Exception &e) {
                        e.ReportException();
                    }
                });
            }
        }
    }
    if (auto propPlacement = Base::freecad_dynamic_cast<App::PropertyPlacement>(
                getObject()->getPropertyByName("Placement")))
    {
        if (getObject()->getExtensionByType<Part::AttachExtension>(true)
                || (!propPlacement->testStatus(App::Property::Hidden)
                    && !propPlacement->testStatus(App::Property::ReadOnly))) {
            if (auto cmd = Gui::Application::Instance->commandManager().getCommandByName("Part_EditAttachment"))
            {
                cmd->initAction();
                if (auto action = cmd->getAction()) {
                    if (auto act = action->action())
                        menu->addAction(act);
                }
            }
        }
    }
    inherited::setupContextMenu(menu, receiver, member);
}

// ----------------------------------------------------------------------------

void ViewProviderShapeBuilder::buildNodes(const App::Property* , std::vector<SoNode*>& ) const
{
}

void ViewProviderShapeBuilder::createShape(const App::Property* , SoSeparator* ) const
{
}
