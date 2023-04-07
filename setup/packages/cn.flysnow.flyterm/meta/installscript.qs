/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the FOO module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

function Component()
{
    installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);    
}

Component.prototype.isDefault = function()
{
    return true;
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
        if (systemInfo.productType === "windows")
        {
            component.addOperation("CreateShortcut", "@TargetDir@/FlyTerm.exe", "@UserStartMenuProgramsPath@/FlyTerm1.0/FlyTerm1.0.lnk",
                "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/FlyTerm.exe",
            "iconId=0", "description=Run FlyTerm1.0");
            component.addOperation("CreateShortcut", "@TargetDir@/uninstall.exe", "@UserStartMenuProgramsPath@/FlyTerm1.0/uninstall FlyTerm1.0.lnk",
                "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/uninstall.exe",
            "iconId=0", "description=Uninstall FlyTerm1.0");
            component.addOperation("CreateShortcut", "@TargetDir@/FlyTerm.exe", "@DesktopDir@/FlyTerm1.0.lnk",
                "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/FlyTerm.exe",
            "iconId=0", "description=Run FlyTerm1.0");
        }
        else if(systemInfo.productType === "osx")
        {
            var fileName = QDesktopServices.storageLocation(QDesktopServices.DesktopLocation) + "/FlyTerm1.0";
            component.addOperation("CreateLink", fileName, "@TargetDir@/FlyTerm.app");
        }
        else
        {
		    var fileName = QDesktopServices.storageLocation(QDesktopServices.DesktopLocation) + "/FlyTerm.desktop";
		    component.addOperation("CreateDesktopEntry", fileName, "Name=FlyTerm1.0\nGenericName=FlyTerm1.0\nExec=@TargetDir@/start\nTerminal=false\nIcon=@TargetDir@/FlyTerm.png\nType=Application\nCategoreis=KDE");
		}
    } catch (e) {
        console.log(e);
    }
}
