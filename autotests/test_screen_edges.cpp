/********************************************************************
KWin - the KDE window manager
This file is part of the KDE project.

Copyright (C) 2014 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
// kwin
#include "../atoms.h"
#include "../cursor.h"
#include "../input.h"
#include "../main.h"
#include "../screenedge.h"
#include "../screens.h"
#include "../utils.h"
#include "../virtualdesktops.h"
#include "../xcbutils.h"
#include "mock_client.h"
#include "mock_screens.h"
#include "mock_workspace.h"
// Frameworks
#include <KConfigGroup>
// Qt
#include <QtTest/QtTest>
// xcb
#include <xcb/xcb.h>
Q_DECLARE_METATYPE(KWin::ElectricBorder)

namespace KWin
{

Atoms* atoms;
int screen_number = 0;

Cursor *Cursor::s_self = nullptr;
static QPoint s_cursorPos = QPoint();
QPoint Cursor::pos()
{
    return s_cursorPos;
}

void Cursor::setPos(const QPoint &pos)
{
    s_cursorPos = pos;
}

void Cursor::setPos(int x, int y)
{
    setPos(QPoint(x, y));
}

void Cursor::startMousePolling()
{
}
void Cursor::stopMousePolling()
{
}

InputRedirection *InputRedirection::s_self = nullptr;

void InputRedirection::registerShortcut(const QKeySequence &shortcut, QAction *action)
{
    Q_UNUSED(shortcut)
    Q_UNUSED(action)
}

void InputRedirection::registerAxisShortcut(Qt::KeyboardModifiers modifiers, PointerAxisDirection axis, QAction *action)
{
   Q_UNUSED(modifiers)
   Q_UNUSED(axis)
   Q_UNUSED(action)
}

void updateXTime()
{
}

Application::OperationMode Application::operationMode() const
{
    return OperationModeX11;
}

class TestObject : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    bool callback(ElectricBorder border);
Q_SIGNALS:
    void gotCallback(KWin::ElectricBorder);
};

bool TestObject::callback(KWin::ElectricBorder border)
{
    emit gotCallback(border);
    return true;
}

}

class TestScreenEdges : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    void testInit();
    void testCreatingInitialEdges();
    void testCallback();
    void testPushBack_data();
    void testPushBack();
    void testFullScreenBlocking();
};

void TestScreenEdges::initTestCase()
{
    KWin::atoms = new KWin::Atoms;
    qRegisterMetaType<KWin::ElectricBorder>();
}

void TestScreenEdges::cleanupTestCase()
{
    delete KWin::atoms;
}

void TestScreenEdges::init()
{
    using namespace KWin;
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    Screens::create();

    auto vd = VirtualDesktopManager::create();
    vd->setConfig(config);
    vd->load();
    auto s = ScreenEdges::create();
    s->setConfig(config);
}

void TestScreenEdges::cleanup()
{
    using namespace KWin;
    delete ScreenEdges::self();
    delete VirtualDesktopManager::self();
    delete Screens::self();
}

void TestScreenEdges::testInit()
{
    using namespace KWin;
    auto s = ScreenEdges::self();
    s->init();
    QCOMPARE(s->isDesktopSwitching(), false);
    QCOMPARE(s->isDesktopSwitchingMovingClients(), false);
    QCOMPARE(s->timeThreshold(), 150);
    QCOMPARE(s->reActivationThreshold(), 350);
    QCOMPARE(s->cursorPushBackDistance(), QSize(1, 1));
    QCOMPARE(s->actionTopLeft(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionTop(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionTopRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottomRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottom(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottomLeft(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionLeft(), ElectricBorderAction::ElectricActionNone);

    QList<Edge*> edges = s->findChildren<Edge*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(edges.size(), 8);
    for (auto e : edges) {
        QVERIFY(!e->isReserved());
        QVERIFY(e->inherits("KWin::WindowBasedEdge"));
        QVERIFY(!e->inherits("KWin::AreaBasedEdge"));
        QVERIFY(!e->client());
        QVERIFY(!e->isApproaching());
    }
    Edge *te = edges.at(0);
    QVERIFY(te->isCorner());
    QVERIFY(!te->isScreenEdge());
    QVERIFY(te->isLeft());
    QVERIFY(te->isTop());
    QVERIFY(!te->isRight());
    QVERIFY(!te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricTopLeft);
    te = edges.at(1);
    QVERIFY(te->isCorner());
    QVERIFY(!te->isScreenEdge());
    QVERIFY(te->isLeft());
    QVERIFY(!te->isTop());
    QVERIFY(!te->isRight());
    QVERIFY(te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricBottomLeft);
    te = edges.at(2);
    QVERIFY(!te->isCorner());
    QVERIFY(te->isScreenEdge());
    QVERIFY(te->isLeft());
    QVERIFY(!te->isTop());
    QVERIFY(!te->isRight());
    QVERIFY(!te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricLeft);
    te = edges.at(3);
    QVERIFY(te->isCorner());
    QVERIFY(!te->isScreenEdge());
    QVERIFY(!te->isLeft());
    QVERIFY(te->isTop());
    QVERIFY(te->isRight());
    QVERIFY(!te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricTopRight);
    te = edges.at(4);
    QVERIFY(te->isCorner());
    QVERIFY(!te->isScreenEdge());
    QVERIFY(!te->isLeft());
    QVERIFY(!te->isTop());
    QVERIFY(te->isRight());
    QVERIFY(te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricBottomRight);
    te = edges.at(5);
    QVERIFY(!te->isCorner());
    QVERIFY(te->isScreenEdge());
    QVERIFY(!te->isLeft());
    QVERIFY(!te->isTop());
    QVERIFY(te->isRight());
    QVERIFY(!te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricRight);
    te = edges.at(6);
    QVERIFY(!te->isCorner());
    QVERIFY(te->isScreenEdge());
    QVERIFY(!te->isLeft());
    QVERIFY(te->isTop());
    QVERIFY(!te->isRight());
    QVERIFY(!te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricTop);
    te = edges.at(7);
    QVERIFY(!te->isCorner());
    QVERIFY(te->isScreenEdge());
    QVERIFY(!te->isLeft());
    QVERIFY(!te->isTop());
    QVERIFY(!te->isRight());
    QVERIFY(te->isBottom());
    QCOMPARE(te->border(), ElectricBorder::ElectricBottom);

    // we shouldn't have any x windows, though
    QCOMPARE(s->windows().size(), 0);
}

void TestScreenEdges::testCreatingInitialEdges()
{
    using namespace KWin;
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    config->group("Windows").writeEntry("ElectricBorders", 2/*ElectricAlways*/);
    config->sync();

    auto s = ScreenEdges::self();
    s->setConfig(config);
    s->init();
    // we don't have multiple desktops, so it's returning false
    QCOMPARE(s->isDesktopSwitching(), true);
    QCOMPARE(s->isDesktopSwitchingMovingClients(), true);
    QCOMPARE(s->actionTopLeft(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionTop(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionTopRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottomRight(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottom(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionBottomLeft(), ElectricBorderAction::ElectricActionNone);
    QCOMPARE(s->actionLeft(), ElectricBorderAction::ElectricActionNone);

    QEXPECT_FAIL("", "needs fixing", Continue);
    QCOMPARE(s->windows().size(), 0);

    // set some reasonable virtual desktops
    config->group("Desktops").writeEntry("Number", 4);
    config->sync();
    auto vd = VirtualDesktopManager::self();
    vd->setConfig(config);
    vd->load();
    QCOMPARE(vd->count(), 4u);
    QCOMPARE(vd->grid().width(), 2);
    QCOMPARE(vd->grid().height(), 2);

    // approach windows for edges not created as screen too small
    s->updateLayout();
    auto edgeWindows = s->windows();
    QCOMPARE(edgeWindows.size(), 12);

    auto testWindowGeometry = [&](int index) {
        Xcb::WindowGeometry geo(edgeWindows[index]);
        return geo.rect();
    };
    QRect sg = screens()->geometry();
    const int co = s->cornerOffset();
    QList<QRect> expectedGeometries{
        QRect(0, 0, 1, 1),
        QRect(0, 0, co, co),
        QRect(0, sg.bottom(), 1, 1),
        QRect(0, sg.height() - co, co, co),
        QRect(0, co, 1, sg.height() - co*2),
//         QRect(0, co * 2 + 1, co, sg.height() - co*4),
        QRect(sg.right(), 0, 1, 1),
        QRect(sg.right() - co + 1, 0, co, co),
        QRect(sg.right(), sg.bottom(), 1, 1),
        QRect(sg.right() - co + 1, sg.bottom() - co + 1, co, co),
        QRect(sg.right(), co, 1, sg.height() - co*2),
//         QRect(sg.right() - co + 1, co * 2, co, sg.height() - co*4),
        QRect(co, 0, sg.width() - co * 2, 1),
//         QRect(co * 2, 0, sg.width() - co * 4, co),
        QRect(co, sg.bottom(), sg.width() - co * 2, 1),
//         QRect(co * 2, sg.height() - co, sg.width() - co * 4, co)
    };
    for (int i = 0; i < 12; ++i) {
        QCOMPARE(testWindowGeometry(i), expectedGeometries.at(i));
    }
    QList<Edge*> edges = s->findChildren<Edge*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(edges.size(), 8);
    for (auto e : edges) {
        QVERIFY(e->isReserved());
    }

    static_cast<MockScreens*>(screens())->setGeometries(QList<QRect>{QRect{0, 0, 1024, 768}});
    QSignalSpy changedSpy(screens(), SIGNAL(changed()));
    QVERIFY(changedSpy.isValid());
    // first is before it's updated
    QVERIFY(changedSpy.wait());
    // second is after it's updated
    QVERIFY(changedSpy.wait());

    // let's update the layout and verify that we have edges
    s->recreateEdges();
    edgeWindows = s->windows();
    QCOMPARE(edgeWindows.size(), 16);
    sg = screens()->geometry();
    expectedGeometries = QList<QRect>{
        QRect(0, 0, 1, 1),
        QRect(0, 0, co, co),
        QRect(0, sg.bottom(), 1, 1),
        QRect(0, sg.height() - co, co, co),
        QRect(0, co, 1, sg.height() - co*2),
        QRect(0, co * 2 + 1, co, sg.height() - co*4),
        QRect(sg.right(), 0, 1, 1),
        QRect(sg.right() - co + 1, 0, co, co),
        QRect(sg.right(), sg.bottom(), 1, 1),
        QRect(sg.right() - co + 1, sg.bottom() - co + 1, co, co),
        QRect(sg.right(), co, 1, sg.height() - co*2),
        QRect(sg.right() - co + 1, co * 2, co, sg.height() - co*4),
        QRect(co, 0, sg.width() - co * 2, 1),
        QRect(co * 2, 0, sg.width() - co * 4, co),
        QRect(co, sg.bottom(), sg.width() - co * 2, 1),
        QRect(co * 2, sg.height() - co, sg.width() - co * 4, co)
    };
    for (int i = 0; i < 16; ++i) {
        QCOMPARE(testWindowGeometry(i), expectedGeometries.at(i));
    }

    // disable desktop switching again
    config->group("Windows").writeEntry("ElectricBorders", 1/*ElectricMoveOnly*/);
    s->reconfigure();
    QCOMPARE(s->isDesktopSwitching(), false);
    QCOMPARE(s->isDesktopSwitchingMovingClients(), true);
    QCOMPARE(s->windows().size(), 0);
    edges = s->findChildren<Edge*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(edges.size(), 8);
    for (int i = 0; i < 8; ++i) {
        auto e = edges.at(i);
        QVERIFY(!e->isReserved());
        QCOMPARE(e->approachGeometry(), expectedGeometries.at(i*2+1));
    }
}

void TestScreenEdges::testCallback()
{
    using namespace KWin;
    MockWorkspace ws;
    static_cast<MockScreens*>(screens())->setGeometries(QList<QRect>{QRect{0, 0, 1024, 768}, QRect{200, 768, 1024, 768}});
    QSignalSpy changedSpy(screens(), SIGNAL(changed()));
    QVERIFY(changedSpy.isValid());
    // first is before it's updated
    QVERIFY(changedSpy.wait());
    // second is after it's updated
    QVERIFY(changedSpy.wait());
    auto s = ScreenEdges::self();
    s->init();
    TestObject callback;
    QSignalSpy spy(&callback, SIGNAL(gotCallback(KWin::ElectricBorder)));
    QVERIFY(spy.isValid());
    s->reserve(ElectricLeft, &callback, "callback");
    s->reserve(ElectricTopLeft, &callback, "callback");
    s->reserve(ElectricTop, &callback, "callback");
    s->reserve(ElectricTopRight, &callback, "callback");
    s->reserve(ElectricRight, &callback, "callback");
    s->reserve(ElectricBottomRight, &callback, "callback");
    s->reserve(ElectricBottom, &callback, "callback");
    s->reserve(ElectricBottomLeft, &callback, "callback");

    QList<Edge*> edges = s->findChildren<Edge*>(QString(), Qt::FindDirectChildrenOnly);
    QCOMPARE(edges.size(), 10);
    for (auto e: edges) {
        QVERIFY(e->isReserved());
    }
    auto it = std::find_if(edges.constBegin(), edges.constEnd(), [](Edge *e) {
        return e->isScreenEdge() && e->isLeft() && e->approachGeometry().bottom() < 768;
    });
    QVERIFY(it != edges.constEnd());

    xcb_enter_notify_event_t event;
    auto setPos = [&event] (const QPoint &pos) {
        Cursor::setPos(pos);
        event.root_x = pos.x();
        event.root_y = pos.y();
        event.event_x = pos.x();
        event.event_y = pos.y();
    };
    event.root = XCB_WINDOW_NONE;
    event.child = XCB_WINDOW_NONE;
    event.event = static_cast<WindowBasedEdge*>(*it)->window();
    event.same_screen_focus = 1;
    event.time = QDateTime::currentMSecsSinceEpoch();
    setPos(QPoint(0, 50));
    QVERIFY(s->isEntered(&event));
    // doesn't trigger as the edge was not triggered yet
    QVERIFY(spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 50));

    // test doesn't trigger due to too much offset
    QTest::qWait(160);
    setPos(QPoint(0, 100));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 100));

    // doesn't trigger as we are waiting too long already
    QTest::qWait(200);
    setPos(QPoint(0, 101));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 101));

    // doesn't activate as we are waiting too short
    QTest::qWait(50);
    setPos(QPoint(0, 100));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 100));

    // and this one triggers
    QTest::qWait(110);
    setPos(QPoint(0, 101));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(!spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 101));

    // now let's try to trigger again
    QTest::qWait(100);
    setPos(QPoint(0, 100));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(Cursor::pos(), QPoint(1, 100));
    // it's still under the reactivation
    QTest::qWait(50);
    setPos(QPoint(0, 100));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(Cursor::pos(), QPoint(1, 100));
    // now it should trigger again
    QTest::qWait(250);
    setPos(QPoint(0, 100));
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.first().first().value<ElectricBorder>(), ElectricLeft);
    QCOMPARE(spy.last().first().value<ElectricBorder>(), ElectricLeft);
    QCOMPARE(Cursor::pos(), QPoint(1, 100));

    // let's disable pushback
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    config->group("Windows").writeEntry("ElectricBorderPushbackPixels", 0);
    config->sync();
    s->setConfig(config);
    s->reconfigure();
    // it should trigger directly
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.at(0).first().value<ElectricBorder>(), ElectricLeft);
    QCOMPARE(spy.at(1).first().value<ElectricBorder>(), ElectricLeft);
    QCOMPARE(spy.at(2).first().value<ElectricBorder>(), ElectricLeft);
    QCOMPARE(Cursor::pos(), QPoint(0, 100));

    // now let's unreserve again
    s->unreserve(ElectricTopLeft, &callback);
    s->unreserve(ElectricTop, &callback);
    s->unreserve(ElectricTopRight, &callback);
    s->unreserve(ElectricRight, &callback);
    s->unreserve(ElectricBottomRight, &callback);
    s->unreserve(ElectricBottom, &callback);
    s->unreserve(ElectricBottomLeft, &callback);
    s->unreserve(ElectricLeft, &callback);
    for (auto e: s->findChildren<Edge*>(QString(), Qt::FindDirectChildrenOnly)) {
        QVERIFY(!e->isReserved());
    }
}

void TestScreenEdges::testPushBack_data()
{
    QTest::addColumn<KWin::ElectricBorder>("border");
    QTest::addColumn<int>("pushback");
    QTest::addColumn<QPoint>("trigger");
    QTest::addColumn<QPoint>("expected");

    QTest::newRow("topleft-3") << KWin::ElectricTopLeft << 3 << QPoint(0, 0) << QPoint(3, 3);
    QTest::newRow("top-5") << KWin::ElectricTop << 5 << QPoint(50, 0) << QPoint(50, 5);
    QTest::newRow("toprigth-2") << KWin::ElectricTopRight << 2 << QPoint(99, 0) << QPoint(97, 2);
    QTest::newRow("right-10") << KWin::ElectricRight << 10 << QPoint(99, 50) << QPoint(89, 50);
    QTest::newRow("bottomright-5") << KWin::ElectricBottomRight << 5 << QPoint(99, 99) << QPoint(94, 94);
    QTest::newRow("bottom-10") << KWin::ElectricBottom << 10 << QPoint(50, 99) << QPoint(50, 89);
    QTest::newRow("bottomleft-3") << KWin::ElectricBottomLeft << 3 << QPoint(0, 99) << QPoint(3, 96);
    QTest::newRow("left-10") << KWin::ElectricLeft << 10 << QPoint(0, 50) << QPoint(10, 50);
    QTest::newRow("invalid") << KWin::ElectricLeft << 10 << QPoint(50, 0) << QPoint(50, 0);
}

void TestScreenEdges::testPushBack()
{
    using namespace KWin;
    QFETCH(int, pushback);
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    config->group("Windows").writeEntry("ElectricBorderPushbackPixels", pushback);
    config->sync();

    // TODO: add screens

    auto s = ScreenEdges::self();
    s->setConfig(config);
    s->init();
    TestObject callback;
    QSignalSpy spy(&callback, SIGNAL(gotCallback(KWin::ElectricBorder)));
    QVERIFY(spy.isValid());
    QFETCH(ElectricBorder, border);
    s->reserve(border, &callback, "callback");

    QFETCH(QPoint, trigger);
    Cursor::setPos(trigger);
    xcb_enter_notify_event_t event;
    event.root_x = trigger.x();
    event.root_y = trigger.y();
    event.event_x = trigger.x();
    event.event_y = trigger.y();
    event.root = XCB_WINDOW_NONE;
    event.child = XCB_WINDOW_NONE;
    event.event = s->windows().first();
    event.same_screen_focus = 1;
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    QTEST(Cursor::pos(), "expected");
}

void TestScreenEdges::testFullScreenBlocking()
{
    using namespace KWin;
    MockWorkspace ws;
    Client client(&ws);
    auto config = KSharedConfig::openConfig(QString(), KConfig::SimpleConfig);
    config->group("Windows").writeEntry("ElectricBorderPushbackPixels", 1);
    config->sync();

    auto s = ScreenEdges::self();
    s->setConfig(config);
    s->init();
    TestObject callback;
    QSignalSpy spy(&callback, SIGNAL(gotCallback(KWin::ElectricBorder)));
    QVERIFY(spy.isValid());
    s->reserve(KWin::ElectricLeft, &callback, "callback");
    s->reserve(KWin::ElectricBottomRight, &callback, "callback");
    // currently there is no active client yet, so check blocking shouldn't do anything
    emit s->checkBlocking();

    xcb_enter_notify_event_t event;
    Cursor::setPos(0, 50);
    event.root_x = 0;
    event.root_y = 50;
    event.event_x = 0;
    event.event_y = 50;
    event.root = XCB_WINDOW_NONE;
    event.child = XCB_WINDOW_NONE;
    event.event = s->windows().first();
    event.same_screen_focus = 1;
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 50));

    client.setGeometry(screens()->geometry());
    client.setActive(true);
    client.setFullScreen(true);
    ws.setActiveClient(&client);
    emit s->checkBlocking();
    // the signal doesn't trigger for corners, let's go over all windows just to be sure that it doesn't call for corners
    for (auto e: s->findChildren<Edge*>()) {
        e->checkBlocking();
    }
    // calling again should not trigger
    QTest::qWait(160);
    Cursor::setPos(0, 50);
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    // and no pushback
    QCOMPARE(Cursor::pos(), QPoint(0, 50));

    // let's make the client not fullscreen, which should trigger
    client.setFullScreen(false);
    emit s->checkBlocking();
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(!spy.isEmpty());
    QCOMPARE(Cursor::pos(), QPoint(1, 50));

    // let's make the client fullscreen again, but with a geometry not intersecting the left edge
    client.setFullScreen(true);
    client.setGeometry(client.geometry().translated(10, 0));
    emit s->checkBlocking();
    spy.clear();
    Cursor::setPos(0, 50);
    event.time = QDateTime::currentMSecsSinceEpoch();
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    // and a pushback
    QCOMPARE(Cursor::pos(), QPoint(1, 50));

    // just to be sure, let's set geometry back
    client.setGeometry(screens()->geometry());
    emit s->checkBlocking();
    Cursor::setPos(0, 50);
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    // and no pushback
    QCOMPARE(Cursor::pos(), QPoint(0, 50));

    // the corner should always trigger
    s->unreserve(KWin::ElectricLeft, &callback);
    event.event_x = 99;
    event.event_y = 99;
    event.root_x = 99;
    event.root_y = 99;
    event.event = s->windows().first();
    event.time = QDateTime::currentMSecsSinceEpoch();
    Cursor::setPos(99, 99);
    QVERIFY(s->isEntered(&event));
    QVERIFY(spy.isEmpty());
    // and pushback
    QCOMPARE(Cursor::pos(), QPoint(98, 98));
    QTest::qWait(160);
    event.time = QDateTime::currentMSecsSinceEpoch();
    Cursor::setPos(99, 99);
    QVERIFY(s->isEntered(&event));
    QVERIFY(!spy.isEmpty());
}

QTEST_MAIN(TestScreenEdges)
#include "test_screen_edges.moc"