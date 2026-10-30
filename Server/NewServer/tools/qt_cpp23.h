#pragma once

#ifndef Q_MOC_RUN
#include <QtCore/qglobal.h>
#if defined(Q_CC_MSVC) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <span>
// Qt 5's compiler detection assumes Microsoft's nonstandard stdext iterators
// still exist. Keep Qt's bounded algorithm destinations using standard spans.
#undef QT_MAKE_CHECKED_ARRAY_ITERATOR
#define QT_MAKE_CHECKED_ARRAY_ITERATOR(pointer, count) (std::span{(pointer), static_cast<std::size_t>(count)}.begin())
#undef QT_MAKE_UNCHECKED_ARRAY_ITERATOR
#define QT_MAKE_UNCHECKED_ARRAY_ITERATOR(pointer) (pointer)
#endif
#endif
