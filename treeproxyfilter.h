#pragma once

#include <QSortFilterProxyModel>

class TreeProxyFilter : public QSortFilterProxyModel
{
public:
	TreeProxyFilter(QObject *parent = NULL);
protected:
	bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	bool filterAcceptsRowSelf(int sourceRow, const QModelIndex &sourceParent) const;
	bool hasAcceptedChildren(int sourceRow, const QModelIndex &sourceParent) const;
};