/*
    Copyright © 2017 by The qTox Project Contributors

    This file is part of qTox, a Qt-based graphical interface for Tox.

    qTox is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qTox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qTox.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "windowsshutdown.h"

#include <QDebug>
#include <QLibrary>
#include <QMainWindow>
#include <QString>

#include <windows.h>

namespace Platform
{

/*!
 * \brief Define reason to block session shutdown on Windows, allowing qTox to save settings
 *
 * Since Windows Vista it's not enougth to handle WM_QUERYENDSESSION
 * to block session shutdown. If main window is hidden (when qTox is
 * minimized to tray), WM_QUERYENDSESSION result is ignored.
 * ShutdownBlockReasonCreate() API has to be used to specify user
 * readable reason to block session shutdown.
 *
 * \see https://msdn.microsoft.com/en-us/library/ms700677(v=vs.85).aspx
 */
void blockWindowsShutdown(QMainWindow *mainWindow, const QString& reason)
{
    // Due to bug [0] in mingw64 when building x86_64 binary, ShutdownBlockReasonCreate
    // symbol is missing, so we have to load it on run time.
    // [0] https://sourceforge.net/p/mingw-w64/bugs/526/
    QLibrary user32(QStringLiteral("User32"));

    if (!user32.load()) {
        qCritical() << "Failed to load User32.dll:" << user32.errorString();
        return;
    }

    using ShutdownBlockReasonCreatePtr = bool (*)(HWND, LPCWSTR);
    ShutdownBlockReasonCreatePtr const shutdownBlockReasonCreate =
            reinterpret_cast<ShutdownBlockReasonCreatePtr>(user32.resolve("ShutdownBlockReasonCreate"));

    if (!shutdownBlockReasonCreate) {
        qCritical() << "Failed to resolve ShutdownBlockReasonCreate function";
        return;
    }

    if(!shutdownBlockReasonCreate(reinterpret_cast<HWND>(mainWindow->winId()),
                                  reinterpret_cast<LPCWSTR>(reason.utf16()))) {
        qCritical() << "Failed to create Windows shutdown block reason:" << GetLastError();
    }
}

} // namespace Platform
