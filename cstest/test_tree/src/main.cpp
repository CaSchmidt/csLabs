#include <cstdio>
#include <cstdlib>

#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>

#include <csQt/csAbstractTreeItem.h>
#include <csQt/csTreeModel.h>

class TreeItem : public csAbstractTreeItem {
public:
  TreeItem(csAbstractTreeItem *parent = 0)
    : csAbstractTreeItem(parent)
  {
  }

  ~TreeItem()
  {
  }

  int columnCount() const
  {
    return 2;
  }

  QVariant data(int column, int role) const
  {
    if( role == Qt::DisplayRole ) {
      if(        column == 0 ) {
        return QString("Key");
      } else if( column == 1 ) {
        return QString("Value");
      }
    }
    return QVariant();
  }
};

class TreeValue : public TreeItem {
public:
  TreeValue(const QString& key, const QVariant& value,
            TreeItem *parent = 0)
    : TreeItem(parent)
    , _key(key)
    , _value(value)
  {
  }

  ~TreeValue()
  {
  }

  QVariant data(int column, int role) const
  {
    if( role == Qt::DisplayRole ) {
      if(        column == 0 ) {
        return _key;
      } else if( column == 1 ) {
        return _value;
      }
    }
    return QVariant();
  }

private:
  QString _key;
  QVariant _value;
};

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QTreeView *view = new QTreeView();
  view->header()->setVisible(true);
  view->setAlternatingRowColors(true);
  view->setEditTriggers(QAbstractItemView::NoEditTriggers);
  view->setSelectionBehavior(QAbstractItemView::SelectRows);
  view->setSelectionMode(QAbstractItemView::SingleSelection);
  view->setWindowTitle("Tree");
  view->show();

  // Begin Data //////////////////////////////////////////////////////////////

  TreeItem *root = new TreeItem();

  TreeValue *child1 = new TreeValue("child 1", "one");
  root->appendChild(child1);

  TreeValue *child11 = new TreeValue("child 1.1", 1);
  child1->appendChild(child11);
  TreeValue *child12 = new TreeValue("child 1.2", 2);
  child1->appendChild(child12);
  TreeValue *child13 = new TreeValue("child 1.3", 3);
  child1->appendChild(child13);

  TreeValue *child2 = new TreeValue("child 2", "two");
  root->appendChild(child2);

  TreeValue *child21 = new TreeValue("child 2.1", 0.1);
  child2->appendChild(child21);
  TreeValue *child22 = new TreeValue("child 2.2", 0.2);
  child2->appendChild(child22);

  TreeValue *child3 = new TreeValue("child 3", "three");
  root->appendChild(child3);

  // End Data ////////////////////////////////////////////////////////////////

  csTreeModel *model = new csTreeModel(root, view);
  view->setModel(model);
  view->expandAll();

  const int result = app.exec();
  delete view;

  return result;
}
