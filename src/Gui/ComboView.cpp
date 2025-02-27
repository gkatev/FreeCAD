/***************************************************************************
 *   Copyright (c) 2009 Jürgen Riegel <juergen.riegel@web.de>              *
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
# include <QEvent>
# include <QSplitter>
#endif

#include <App/Application.h>
#include "TaskView/TaskView.h"
#include "ComboView.h"
#include "BitmapFactory.h"
#include "DockWindowManager.h"
#include "Document.h"
#include "MainWindow.h"
#include "PropertyView.h"
#include "Tree.h"


using namespace Gui;
using namespace Gui::DockWnd;


/* TRANSLATOR Gui::DockWnd::ComboView */

ComboView::ComboView(bool showModel, Gui::Document* pcDocument, QWidget *parent)
  : DockWindow(pcDocument,parent)
  , oldTabIndex(0)
  , modelIndex(-1)
  , taskIndex(-1)
{
    hGrp = App::GetApplication().GetParameterGroupByPath(
                "User parameter:BaseApp/Preferences/DockWindows/ComboView");

    auto pLayout = new QGridLayout(this);
    pLayout->setSpacing( 0 );
    pLayout->setContentsMargins ( 0, 0, 0, 0 );

    // tabs to switch between Tree/Properties and TaskPanel
    tabs = new QTabWidget ();
    tabs->setObjectName(QString::fromUtf8("combiTab"));
    tabs->setTabPosition(QTabWidget::North);
    pLayout->addWidget( tabs, 0, 0 );

#if QT_VERSION>=QT_VERSION_CHECK(5,4,0)
    tabs->setTabBarAutoHide(true);
#endif

    setShowModel(showModel);
    connect(tabs, qOverload<int>(&QTabWidget::currentChanged),
            this, &ComboView::onCurrentTabChanged);

    // task panel
    taskPanel = new Gui::TaskView::TaskView(this);
    taskIndex = tabs->addTab(taskPanel, tr("Tasks"));

    // task panel
    //projectView = new Gui::ProjectWidget(this);
    //tabs->addTab(projectView, tr("Project"));
}

ComboView::~ComboView()
{
}

bool ComboView::hasTreeView() const
{
    return tree != nullptr;
}

static inline void checkFocus()
{
#ifdef FC_OS_MACOSX
    // Prevent undesired view switch
    getMainWindow()->setFocus();
#endif
}

void ComboView::setShowModel(bool showModel)
{
    setWindowTitle(showModel ? tr("Combo View") : tr("Task View"));
    if (showModel) {
        if(tree) {
            tabs->setCurrentIndex(0);
            return;
        }
        // splitter between tree and property view
        auto splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);

        tree =  new TreePanel("ComboView", this);
        splitter->addWidget(tree);

        // property view
        if(!prop)
            prop = new PropertyView(this);

        modelIndex = tabs->insertTab(0, splitter,tr("Model"));
        modelIndex = 0;
        taskIndex = 1;

        splitter->addWidget(prop);
        prop->show();

        int size1 = hGrp->GetInt("TreeViewSize", 0);
        int size2 = hGrp->GetInt("PropertyViewSize", 0);
        if (size1 || size2) {
            QList<int> sizes;
            sizes.append(size1);
            sizes.append(size2);
            splitter->setSizes(sizes);
        }

        connect(splitter, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved()));

        tabs->setCurrentIndex(0);

    } else if (modelIndex >= 0) {
        tabs->removeTab(modelIndex);
        modelIndex = -1;
        delete prop;
        prop = nullptr;
        delete tree;
        tree = nullptr;
    }
}

void ComboView::onSplitterMoved()
{
    auto splitter = qobject_cast<QSplitter*>(sender());
    if (splitter) {
        auto sizes = splitter->sizes();
        hGrp->SetInt("TreeViewSize", sizes[0]);
        hGrp->SetInt("PropertyViewSize", sizes[1]);
    }
}

void ComboView::showDialog(Gui::TaskView::TaskDialog *dlg)
{
    static QIcon icon = Gui::BitmapFactory().pixmap("edit-edit.svg");

    checkFocus();

    // switch to the TaskView tab
    oldTabIndex = tabs->currentIndex();
    tabs->setCurrentIndex(taskIndex);
    tabs->setTabIcon(taskIndex, icon);
    // set the dialog
    taskPanel->showDialog(dlg);

    // force to show the combo view
    if (modelIndex < 0) {
        if (parentWidget())
            parentWidget()->raise();
    }
}

void ComboView::closeDialog()
{
    // close the dialog
    checkFocus();
    taskPanel->removeDialog();
}

void ComboView::closedDialog()
{
    static QIcon icon = QIcon();

    checkFocus();

    // dialog has been closed
    tabs->setCurrentIndex(oldTabIndex);
    tabs->setTabIcon(taskIndex, icon);
}

void ComboView::showTreeView()
{
    if (modelIndex < 0)
        return;
    checkFocus();

    // switch to the tree view
    tabs->setCurrentIndex(modelIndex);
}

void ComboView::showTaskView()
{
    checkFocus();

    // switch to the task view
    tabs->setCurrentIndex(taskIndex);
}

void ComboView::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        tabs->setTabText(modelIndex, tr("Model"));
        tabs->setTabText(taskIndex, tr("Tasks"));
        //tabs->setTabText(2, tr("Project"));
    }

    DockWindow::changeEvent(e);
}

void ComboView::onCurrentTabChanged(int index)
{
    if (index != taskIndex)
        oldTabIndex = index;
}

#include "moc_ComboView.cpp"
