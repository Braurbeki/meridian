#pragma once
#include <QColor>
#include <QString>

class QApplication;

namespace mer::ui {

/// The application's colour scheme, in one place.
///
/// Review rooms are dark and graded; a light UI next to a picture monitor
/// skews how people judge what they are looking at. Everything here is fixed
/// rather than following the desktop theme for that reason.
namespace theme {

inline QColor window()        { return QColor(0x16, 0x18, 0x1c); }
inline QColor panel()         { return QColor(0x1c, 0x1f, 0x24); }
inline QColor surface()       { return QColor(0x23, 0x26, 0x2c); }
inline QColor raised()        { return QColor(0x2b, 0x2f, 0x36); }
inline QColor border()        { return QColor(0x33, 0x38, 0x41); }

inline QColor text()          { return QColor(0xd8, 0xdb, 0xe0); }
inline QColor textDim()       { return QColor(0x8b, 0x91, 0x9c); }
inline QColor textFaint()     { return QColor(0x5d, 0x63, 0x6e); }

inline QColor accent()        { return QColor(0x4b, 0x9f, 0xd5); }
inline QColor playhead()      { return QColor(0xff, 0x5c, 0x4d); }
inline QColor warning()       { return QColor(0xd9, 0x9a, 0x4e); }

inline QColor videoSegment()      { return QColor(0x33, 0x5c, 0x86); }
inline QColor videoSegmentTop()   { return QColor(0x3f, 0x6f, 0xa0); }
inline QColor audioSegment()      { return QColor(0x2f, 0x6b, 0x5e); }
inline QColor audioSegmentTop()   { return QColor(0x39, 0x82, 0x71); }
inline QColor offlineSegment()    { return QColor(0x6e, 0x3f, 0x3f); }
inline QColor offlineSegmentTop() { return QColor(0x85, 0x4c, 0x4c); }

inline QColor trackEven()     { return QColor(0x1f, 0x22, 0x28); }
inline QColor trackOdd()      { return QColor(0x1b, 0x1e, 0x23); }
inline QColor trackHead()     { return QColor(0x25, 0x29, 0x30); }
inline QColor ruler()         { return QColor(0x14, 0x16, 0x1a); }

} // namespace theme

/// Installs the palette and stylesheet. Call once, before the first window.
void applyTheme(QApplication& app);

} // namespace mer::ui
