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

#include "windowseventfilter.h"

#include <QDebug>

#include <windows.h>

bool WindowsEventFilter::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);

    MSG* const msg = static_cast<MSG*>(message);

    if (msg->message == WM_QUERYENDSESSION) {
        qDebug() << "Received WM_QUERYENDSESSION event";
        bool allowSessionEnd = true; // by default, session termination is not canceled
                                     // by this event handler implementation
        emit queryEndSessionEventReceived(&allowSessionEnd);
        *result = allowSessionEnd;
        return true; // we have handled this event, do not allow default handler.
    }

    return false; // do not filter out, allow others to process messages
}
