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

#include "PreCompiled.h"
#ifndef _PreComp_
# include <QShortcutEvent>
# include <QApplication>
#endif

#include <boost/algorithm/string/predicate.hpp>

#include <Base/Console.h>
#include <Base/Tools.h>
#include "ShortcutManager.h"
#include "Command.h"
#include "Window.h"
#include "Action.h"

FC_LOG_LEVEL_INIT("Gui", true, true)

using namespace Gui;

ShortcutManager::ShortcutManager()
{
    hShortcuts = WindowParameter::getDefaultParameter()->GetGroup("Shortcut");
    hShortcuts->Attach(this);
    hPriorities = hShortcuts->GetGroup("Priorities");
    hPriorities->Attach(this);
    hSetting = hShortcuts->GetGroup("Settings");
    hSetting->Attach(this);
    timeout = hSetting->GetInt("ShortcutTimeout", 300);
    timer.setSingleShot(true);

    QObject::connect(&timer, &QTimer::timeout, [this](){onTimer();});

    for (const auto &v : hPriorities->GetIntMap())
        priorities[v.first] = v.second;

    QApplication::instance()->installEventFilter(this);
}

ShortcutManager::~ShortcutManager()
{
    hShortcuts->Detach(this);
    hSetting->Detach(this);
    hPriorities->Detach(this);
}

static ShortcutManager *Instance;
ShortcutManager *ShortcutManager::instance()
{
    if (!Instance)
        Instance = new ShortcutManager;
    return Instance;
}

void ShortcutManager::destroy()
{
    delete Instance;
    Instance = nullptr;
}

void ShortcutManager::OnChange(Base::Subject<const char*> &src, const char *reason)
{
    if (hSetting == &src) {
        if (boost::equals(reason, "ShortcutTimeout"))
            timeout = hSetting->GetInt("ShortcutTimeout");
        return;
    }

    if (busy)
        return;

    if (hPriorities == &src) {
        int p = hPriorities->GetInt(reason, 0);
        if (p == 0)
            priorities.erase(reason);
        else
            priorities[reason] = p;
        return;
    }

    Base::StateLocker lock(busy);
    auto cmd = Application::Instance->commandManager().getCommandByName(reason);
    if (cmd) {
        auto accel = cmd->getAccel();
        if (!accel) accel = "";
        cmd->setShortcut(getShortcut(reason, accel));
    }
}

void ShortcutManager::reset(const char *cmd)
{
    hShortcuts->RemoveASCII(cmd);
    hPriorities->RemoveInt(cmd);
}

void ShortcutManager::resetAll()
{
    Base::StateLocker lock(busy);
    hShortcuts->Clear();
    hPriorities->Clear();
    for (auto cmd : Application::Instance->commandManager().getAllCommands()) {
        if (cmd->getAction()) {
            auto accel = cmd->getAccel();
            if (!accel) accel = "";
            cmd->setShortcut(getShortcut(nullptr, accel));
        }
    }
}

QString ShortcutManager::getShortcut(const char *cmdName, const char *accel)
{
    if (!accel) {
        if (auto cmd = Application::Instance->commandManager().getCommandByName(cmdName)) {
            accel = cmd->getAccel();
            if (!accel)
                accel = "";
        }
    }
    QString shortcut;
    if (cmdName)
        shortcut = QString::fromLatin1(hShortcuts->GetASCII(cmdName, accel).c_str());
    else
        shortcut = QString::fromLatin1(accel);
    return QKeySequence(shortcut).toString(QKeySequence::NativeText);
}

void ShortcutManager::setShortcut(const char *cmdName, const char *accel)
{
    if (cmdName && cmdName[0]) {
        if (!accel)
            accel = "";
        if (auto cmd = Application::Instance->commandManager().getCommandByName(cmdName)) {
            auto defaultAccel = cmd->getAccel();
            if (!defaultAccel)
               defaultAccel = "";
           if (QKeySequence(QString::fromLatin1(accel)) == QKeySequence(QString::fromLatin1(defaultAccel))) {
                hShortcuts->RemoveASCII(cmdName);
                return;
           }
        }
        hShortcuts->SetASCII(cmdName, accel);

        if (accel[0]) {
            int priority = 0;
            for (const auto &v :  getActionsByShortcut(QString::fromLatin1(accel))) {
                int p = getPriority(v.first.constData());
                if (v.first != cmdName && priority <= p)
                    priority = p+1;
            }
            setPriority(cmdName, priority);
        }
    }
}

bool ShortcutManager::checkShortcut(QObject *o, const QKeySequence &key)
{
    auto focus = QApplication::focusWidget();
    if (!focus)
        return false;
    auto action = qobject_cast<QAction*>(o);
    if (!action)
        return false;

    const auto &index = actionMap.get<1>();
    auto iter = index.lower_bound(ActionKey(key));
    if (iter == index.end())
        return false;

    auto it = iter;
    // skip to the first not exact matched key
    for (; it != index.end() && key == it->key.shortcut; ++it);

    // check for potential partial match, i.e. longer key sequences
    bool flush = true;
    for (; it != index.end(); ++it) {
        if (key.matches(it->key.shortcut) == QKeySequence::NoMatch)
            break;
        if (it->action && it->action->isEnabled()) {
            flush = false;
            break;
        }
    }
                
    int count = 0;
    for (it = iter; it != index.end() && key == it->key.shortcut; ++it) {
        if (it->action && it->action->isEnabled()) {
            if (!flush) {
                // temporary disable the action so that we can try potential
                // match with further keystrokes.
                it->action->setEnabled(false);
            }
            pendingActions.emplace_back(it->action, key.count(), getPriority(it->key.name));
            ++count;
        }
    }
    if (!count) {
        // action not found in the map, shouldn't happen!
        pendingActions.emplace_back(action, key.count(), 0);
    }
    if (flush) {
        // We'll flush now because there is no poential match with further
        // keystrokes, so no need to wait for timer.
        onTimer();
        return true;
    }

    // Qt's shortcut state machine favors shortest match (which is ridiculous,
    // unless I'm mistaken?). We'll do longest match. We've disabled all
    // shortcuts that can match the current key sequence. Now reply the sequence
    // and wait for the next keystroke.
    for (int i=0; i<key.count(); ++i) {
        int k = key[i];
        Qt::KeyboardModifiers modifiers;
        if (k & Qt::SHIFT)
            modifiers |= Qt::ShiftModifier;
        if (k & Qt::CTRL)
            modifiers |= Qt::ControlModifier;
        if (k & Qt::ALT)
            modifiers |= Qt::AltModifier;
        if (k & Qt::META)
            modifiers |= Qt::MetaModifier;
        k &= ~(Qt::SHIFT|Qt::CTRL|Qt::ALT|Qt::META);
        QKeyEvent *kev = new QKeyEvent(QEvent::KeyPress, k, modifiers, 0, 0, 0);
        QApplication::postEvent(focus, kev);
        kev = new QKeyEvent(QEvent::KeyRelease, k, modifiers, 0, 0, 0);
        QApplication::postEvent(focus, kev);
    }
    timer.start(timeout);
    return true;
}

bool ShortcutManager::eventFilter(QObject *o, QEvent *ev)
{
    switch(ev->type()) {
    case QEvent::Shortcut:
        if (timeout > 0) {
            auto sev = static_cast<QShortcutEvent*>(ev);
            if (checkShortcut(o, sev->key())) {
                // shortcut event handled here, so filter out the event
                return true;
            } else {
                // Not handled. Clear any existing pending actions.
                timer.stop();
                for (const auto &info : pendingActions) {
                    if (info.action)
                        info.action->setEnabled(true);
                }
                pendingActions.clear();
            }
        }
        break;
    case QEvent::ActionChanged:
        if (auto action = qobject_cast<QAction*>(o)) {
            auto &index = actionMap.get<0>();
            auto it = index.find(reinterpret_cast<intptr_t>(action));
            if (action->shortcut().isEmpty()) {
                if (it != index.end())
                    index.erase(it);
                break;
            }

            QByteArray name;
            if (auto fcAction = qobject_cast<Action*>(action->parent())) {
                if (fcAction->command() && fcAction->command()->getName())
                    name = fcAction->command()->getName();
            }
            if (name.isEmpty()) {
                name = action->objectName().size() ?
                    action->objectName().toUtf8() : action->text().toUtf8();
                if (name.isEmpty())
                    name = "~";
                else
                    name = QByteArray("~ ") + name;
            }
            if (it != index.end()) {
                if (it->key.shortcut == action->shortcut() && it->key.name == name)
                    break;
                index.replace(it, {action, name});
                FC_LOG("replace " << name.constData() << ": "
                        <<  action->shortcut().toString().toLatin1().constData());
            } else {
                index.insert({action, name});
                FC_LOG(actionMap.size() << " " << name.constData() << ": "
                        <<  action->shortcut().toString().toLatin1().constData());
            }
        }
        break;
    default:
        break;
    }
    return false;
}

std::vector<std::pair<QByteArray, QAction*>> ShortcutManager::getActionsByShortcut(const QKeySequence &shortcut)
{
    const auto &index = actionMap.get<1>();
    std::vector<std::pair<QByteArray, QAction*>> res;
    std::multimap<int, const ActionData*> map;
    for (auto it = index.lower_bound(ActionKey(shortcut)); it != index.end(); ++it) {
        if (it->key.shortcut != shortcut)
            break;
        if (it->key.name != "~" && it->action)
            map.emplace(getPriority(it->key.name), &(*it));
    }
    for (const auto &v : map)
        res.emplace_back(v.second->key.name, v.second->action);
    return res;
}

void ShortcutManager::setPriority(const std::vector<QByteArray> &actions)
{
    int current = 0;
    for (const auto &name : actions) {
        int p = getPriority(name);
        if (p < current) {
            setPriority(name, current);
            ++current;
        } else
            current = p + 1;
    }
}

int ShortcutManager::getPriority(const char *cmdName)
{
    if (!cmdName)
        return 0;
    auto it = priorities.find(cmdName);
    if (it == priorities.end())
        return 0;
    return it->second;
}

void ShortcutManager::setPriority(const char *cmdName, int p)
{
    if (p == 0)
        hPriorities->RemoveInt(cmdName);
    else
        hPriorities->SetInt(cmdName, p);
}

void ShortcutManager::onTimer()
{
    QAction *found = nullptr;
    int priority = -INT_MAX;
    int seq_length = 0;
    for (const auto &info : pendingActions) {
        if (info.action) {
            info.action->setEnabled(true);
            if (info.seq_length > seq_length || info.priority > priority) {
                priority = info.priority;
                seq_length = info.seq_length;
                found = info.action;
            }
        }
    }
    if (found)
        found->activate(QAction::Trigger);
    pendingActions.clear();
    timer.stop();
}

#include "moc_ShortcutManager.cpp"
